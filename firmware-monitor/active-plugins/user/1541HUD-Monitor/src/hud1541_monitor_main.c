/* 1541HUD V1.0.0 hardware-proven release baseline. */
/*
 * 1541HUD UB4 passive monitor V1.0.0
 *
 * Data path:
 * 1541 write cycles
 * -> PIO0 SM3 deterministic sampling
 * -> DMA11 circular ring
 * -> bus-event decoder
 * -> compact mailbox queue
 * -> USB CDC formatter
 * -> 1541HUD GUI
 *
 * The monitor is passive. It never drives the 1541 bus.
 *
 * Current decoded state:
 * - physical stepper phase and half-track movement
 * - spindle motor state
 * - HOME anchor from DOS track state
 * - write-protect state from DOS RAM
 * - actual hardware density selection D0-D3 from VIA2 PB5/PB6 writes
 */

#include "plugin.h"
#include "1541hud_mailbox.h"
#include "flash_erase.h"

/*
 * V8.1 passive-monitor contract: this plugin claims the exact PIO0/SM3,
 * instruction 26..31 and DMA11 resources it needs before configuring them.
 * The claim API prevents the Fire-24G RGB engine or a future plugin from
 * silently stealing capture hardware.  The passive core never starts ROM
 * serving PIO, so bus GPIOs remain inputs until claimed here.
 */
ORA_DEFINE_USER_PLUGIN(hud1541_probe_main, MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, BUILD_VERSION, 0,8,1);

#define TIMER0_BASE 0x400B0000u
#define TIMER0_RAWL (*(volatile uint32_t *)(TIMER0_BASE + 0x28u))

/* Fire-24G UB4 Monitor harness: UC1 pin 37 (SYNC) enters through the
 * board's 5 V-tolerant SEL_B path, GPIO27.  SEL_D/GPIO24 is deliberately
 * unused on UB4; write-protect belongs solely to UB3 Control. */
#define SIO_BASE 0xD0000000u
#define SIO_GPIO_IN (*(volatile uint32_t *)(SIO_BASE + 0x004u))
#define MASK_SYNC (1u << 27)
#define HUD1541_EVENT_SYNC_DIAG 8u
/* The upper flag byte is a host-selected telemetry subscription. Low flag
 * bits retain their published capture-health meaning. A zero mask is quiet:
 * status snapshots continue, but optional event work is suppressed. */
#define HUD1541_TELEM_SHIFT 8u
#define HUD1541_TELEM_CORE (1u << 0)
#define HUD1541_TELEM_HEADERS (1u << 1)
#define HUD1541_TELEM_METADATA (1u << 2)
#define HUD1541_TELEM_RPM (1u << 3)
#define HUD1541_TELEM_SYNC (1u << 4)
#define HUD1541_TELEM_ALL 0x1Fu
#define HDR_TELEM_INTERVAL 8u
#define HDR_TELEM_DIVIDER_MASK (HDR_TELEM_INTERVAL - 1u)
/* A checksum-valid header identifies the track/density zone.  RPM itself is
 * timed from raw physical SYNC edges across two whole revolutions.  DOS may
 * observe header RAM writes selectively, so treating their recurrence as a
 * spindle clock was the wrong algorithm for the Fire-24G Monitor port. */
#define HDR_RPM_WINDOW_REVOLUTIONS 2u
#define HDR_RPM_LAP_MIN_US 150000u
#define HDR_RPM_LAP_MAX_US 250000u
#define HDR_RPM_MIN_100 24000u
#define HDR_RPM_MAX_100 36000u
/* A startup/reset burst can overflow the tiny raw ring before the consumer
 * enters its steady state. Keep that loss in the counters for diagnosis, but
 * do not leave the RGB LED warning forever after capture has recovered. */
#define HUD1541_LED_WARNING_HOLD_US 1500000u
/* Do not use ORA_LED_MODE_BEACON for the startup colour. Its bounded restore
 * is owned by the core, and a later restore can extinguish a newer persistent
 * Monitor colour. The Monitor owns this explicit boot interval instead. */
#define HUD1541_LED_BOOT_HOLD_US 2500000u
#define HUD1541_LED_POLICY_AUTO 0xA5u
#define HUD1541_LED_POLICY_OFF  0x5Au
#define HUD1541_LED_RESULT_OK   1u
#define HUD1541_LED_RESULT_FAIL 2u
#define HUD1541_NV_SIZE 4096u
#define HUD1541_NV_PAGE 256u
#define HUD1541_FLASH_HELPER_MAX 128u
extern const uint8_t __nv_storage_start[];
extern const uint8_t __flash_erase_fn_start[];
extern const uint8_t __flash_erase_fn_end[];

static void *hud1541_boot_fn(char a, char b) {
 return ORA_BOOTROM_LOOKUP(((uint32_t)(uint8_t)b << 8) | (uint8_t)a,
                           ORA_BOOTROM_FLAG_ARM_SEC);
}

static uint32_t hud1541_irq_disable(void) {
 uint32_t primask;
 __asm volatile("mrs %0, primask \n\t cpsid i" : "=r"(primask) :: "memory");
 return primask;
}

static void hud1541_irq_restore(uint32_t primask) {
 __asm volatile("msr primask, %0" :: "r"(primask) : "memory");
}

/*
 * Monitor owns this sector and currently allocates only NV1.  This is the
 * same private-slot rule used by V8.1 Host-Control where that private range
 * exists. This passive image has one 32KB ROM set and therefore a much
 * smaller RAM-slot geometry, so its safe fallback is a checked *inactive*
 * slot. Never "pick the last slot" blindly -- it may be the active ROM slot.
 */
#define HUD1541_HOST_RAM_SLOTS 170u
static uint8_t hud1541_save_led_policy(ora_lookup_fn_t lookup, uint8_t policy) {
 ora_get_ram_slot_count_fn_t count = lookup(ORA_ID_GET_RAM_SLOT_COUNT);
 ora_get_ram_slot_info_fn_t info = lookup(ORA_ID_GET_RAM_SLOT_INFO);
 ora_get_active_ram_slot_fn_t active = lookup(ORA_ID_GET_ACTIVE_RAM_SLOT);
 ora_enter_exclusive_mode_fn_t enter = lookup(ORA_ID_ENTER_EXCLUSIVE_MODE);
 ora_exit_exclusive_mode_fn_t leave = lookup(ORA_ID_EXIT_EXCLUSIVE_MODE);
 if (!count || !info || !active || !enter || !leave) return 0u;

 uint8_t slots = count(), active_slot = 0u;
 uint32_t slot_size = 0u, base = 0u;
 uint8_t staging_slot;
 if (active(&active_slot) != ORA_RESULT_OK || slots < 2u) return 0u;

 if (slots > HUD1541_HOST_RAM_SLOTS &&
     info(HUD1541_HOST_RAM_SLOTS, NULL, &slot_size, NULL) == ORA_RESULT_OK &&
     slot_size != 0u) {
  uint32_t slots_needed = (HUD1541_NV_SIZE + slot_size - 1u) / slot_size;
  if (slots_needed == 0u || slots_needed > (uint32_t)(slots - HUD1541_HOST_RAM_SLOTS)) return 0u;
  staging_slot = (uint8_t)((uint32_t)slots - slots_needed);
  if (active_slot >= staging_slot) return 0u;
 } else {
  /* Passive Fire-24G Monitor: one served slot, remaining slots are inactive.
   * Use the highest non-active one only after confirming it holds the entire
   * sector. This fallback is deliberately not used by multi-slot Control. */
  staging_slot = (uint8_t)(slots - 1u);
  if (staging_slot == active_slot) staging_slot--;
 }
 if (info(staging_slot, &base, &slot_size, NULL) != ORA_RESULT_OK ||
     slot_size < HUD1541_NV_SIZE + HUD1541_FLASH_HELPER_MAX) return 0u;

 /* Preserve every byte in the sector.  NV is a sector, not a disposable
  * LED-byte scratchpad; future Monitor settings must survive this update. */
 volatile uint8_t *staging = ORA_SRAM_PTR(base);
 for (uint32_t i = 0u; i < HUD1541_NV_SIZE; ++i) staging[i] = __nv_storage_start[i];
 staging[1] = policy;
 connect_internal_flash_fn_t connect = (connect_internal_flash_fn_t)hud1541_boot_fn('I','F');
 flash_exit_xip_fn_t exit_xip = (flash_exit_xip_fn_t)hud1541_boot_fn('E','X');
 flash_range_erase_fn_t erase = (flash_range_erase_fn_t)hud1541_boot_fn('R','E');
 flash_range_program_fn_t program = (flash_range_program_fn_t)hud1541_boot_fn('R','P');
 flash_flush_cache_fn_t flush = (flash_flush_cache_fn_t)hud1541_boot_fn('F','C');
 flash_select_xip_read_mode_fn_t select = (flash_select_xip_read_mode_fn_t)hud1541_boot_fn('X','M');
 if (!connect || !exit_xip || !erase || !program || !flush || !select || enter() != ORA_RESULT_OK) return 0u;
 uint32_t n = ORA_STAGED_FN_SIZE(__flash_erase_fn_start, __flash_erase_fn_end);
 if (n > HUD1541_FLASH_HELPER_MAX) { leave(); return 0u; }
 /* The Monitor has only 512 bytes of runtime stack after its fixed mailbox.
  * Do not put the copied helper on that stack: Monitor main + save + bootrom
  * frames overflow it. The verified inactive staging slot has ample space
  * directly after the 4KB sector image. */
 volatile uint8_t *code = staging + HUD1541_NV_SIZE;
 for (uint32_t i = 0u; i < n; ++i) code[i] = __flash_erase_fn_start[i];
 __asm volatile("dsb \n\t isb" ::: "memory");
 /* Match upstream Host-Control exactly: after internal flash is connected,
  * an interrupt fetched from XIP can fault before the RAM helper restores it. */
 uint32_t primask = hud1541_irq_disable();
 connect();
 hud1541_flash_commit_fn_t commit = ORA_STAGED_FN_PTR(hud1541_flash_commit_fn_t, (uintptr_t)code);
 uint32_t offset = ORA_FLASH_OFFSET(__nv_storage_start);
 commit(exit_xip, erase, program, flush, select, offset, offset, (const uint8_t *)staging,
        HUD1541_NV_SIZE, HUD1541_NV_SIZE, ORA_XIP_CLKDIV());
 hud1541_irq_restore(primask);
 leave();
 return __nv_storage_start[1] == policy;
}
#define PIO0_BASE 0x50200000u
#define PIO_CTRL (*(volatile uint32_t *)(PIO0_BASE + 0x000u))
/* RP2350 peripheral atomic aliases.  Do not RMW PIO CTRL: V8.1 may have
 * other users of the block and the legacy restart bits are not an API here. */
#define PIO_CTRL_SET (*(volatile uint32_t *)(PIO0_BASE + 0x2000u))
#define PIO_CTRL_CLR (*(volatile uint32_t *)(PIO0_BASE + 0x3000u))
#define PIO_FLEVEL (*(volatile uint32_t *)(PIO0_BASE + 0x00Cu))
#define PIO_RXF3 (*(volatile uint32_t *)(PIO0_BASE + 0x02Cu))
#define PIO_INSTR_MEM(n) (*(volatile uint32_t *)(PIO0_BASE + 0x048u + ((n) * 4u)))
#define PIO_SM3_CLKDIV (*(volatile uint32_t *)(PIO0_BASE + 0x110u))
#define PIO_SM3_EXECCTRL (*(volatile uint32_t *)(PIO0_BASE + 0x114u))
#define PIO_SM3_SHIFTCTRL (*(volatile uint32_t *)(PIO0_BASE + 0x118u))
#define PIO_SM3_ADDR (*(volatile uint32_t *)(PIO0_BASE + 0x11Cu))
#define PIO_SM3_INSTR (*(volatile uint32_t *)(PIO0_BASE + 0x120u))
#define PIO_SM3_PINCTRL (*(volatile uint32_t *)(PIO0_BASE + 0x124u))

#define DMA_BASE 0x50000000u
#define DMA_CH 11u
#define CAPTURE_PIO_BLOCK 0u
#define CAPTURE_PIO_SM 3u
#define CAPTURE_PIO_FIRST_INSTR 26u
#define CAPTURE_PIO_INSTR_COUNT 6u

typedef struct {
 volatile uint32_t read_addr;
 volatile uint32_t write_addr;
 volatile uint32_t transfer_count;
 volatile uint32_t ctrl_trig;
} dma_ch_t;

#define DMA11 ((dma_ch_t *)(DMA_BASE + DMA_CH * 0x40u))

#define DMA_EN (1u << 0)
#define DMA_SIZE_32 (2u << 2)
#define DMA_INCR_WRITE (1u << 6)
#define DMA_RING_SIZE(n) (((n) & 0xFu) << 8)
#define DMA_RING_SEL (1u << 12)
#define DMA_CHAIN_TO(x) (((x) & 0xFu) << 13)
#define DMA_TREQ(x) (((x) & 0x3Fu) << 17)
#define DMA_IRQ_QUIET (1u << 23)
#define DMA_BUSY (1u << 24)
#define DREQ_PIO0_RX3 7u

/*
 * Fire-24G Monitor harness:
 *   UC2 pin 23 (/CS2) -> SEL_A -> GPIO26.
 * GPIO25 is SEL_C on this board and is not the VIA select.  Reading the old
 * Fire-24E GPIO25 location left every VIA2 write invisible, which in turn
 * left the HUD without motor, phase, density, or DOS-RAM telemetry.
 */
#define MASK_nCS2 (1u << 26)

#define DMA_COUNT_MASK 0x0fffffffu
#define DMA_RELOAD 0x0fffffffu
#define DMA_TRIGGER_SELF_MAX 0x1fffffffu

#define RAM_LWPT 0x001Eu
#define RAM_HDRID2 0x0016u
#define RAM_HDRID1 0x0017u
#define RAM_HDRTRK 0x0018u
#define RAM_HDRSEC 0x0019u
#define RAM_HDRCHK 0x001Au
#define RAM_DRVST 0x0020u
#define RAM_DRVTRK 0x0022u

#define TRACK_STATE_LAST_TRACK_MASK 0x000000FFu
#define TRACK_STATE_TRACK_WRITE_VALID (1u << 8)
#define TRACK_STATE_POS2_SHIFT 16u
#define TRACK_STATE_POS2_MASK (0xFFu << TRACK_STATE_POS2_SHIFT)
#define TRACK_STATE_POS_VALID (1u << 24)
#define TRACK_STATE_STEPPING (1u << 25)

static uint8_t logical_data_from_gpio(uint32_t v) {
 return (uint8_t)((((v >> 7) & 1u) << 0) |
 (((v >> 6) & 1u) << 1) |
 (((v >> 5) & 1u) << 2) |
 (((v >> 0) & 1u) << 3) |
 (((v >> 1) & 1u) << 4) |
 (((v >> 2) & 1u) << 5) |
 (((v >> 3) & 1u) << 6) |
 (((v >> 4) & 1u) << 7));
}

static uint32_t logical_addr13_from_gpio(uint32_t v) {
 return (((v >> 23) & 1u) << 0) |
 (((v >> 22) & 1u) << 1) |
 (((v >> 21) & 1u) << 2) |
 (((v >> 20) & 1u) << 3) |
 (((v >> 19) & 1u) << 4) |
 (((v >> 18) & 1u) << 5) |
 (((v >> 17) & 1u) << 6) |
 (((v >> 16) & 1u) << 7) |
 (((v >> 15) & 1u) << 8) |
 (((v >> 14) & 1u) << 9) |
 (((v >> 13) & 1u) << 10) |
 (((v >> 11) & 1u) << 11) |
 (((v >> 12) & 1u) << 12);
}

static uint8_t uc2_selected(uint32_t v) {
 /* Hardware-proven T0.0.14 result: CS1 is not required for current decode. */
 return (uint8_t)((v & MASK_nCS2) == 0u);
}

static uint32_t telemetry_mask(volatile hud1541_mailbox_t *m) {
 return (m->flags >> HUD1541_TELEM_SHIFT) & HUD1541_TELEM_ALL;
}

static uint8_t uc2_orb_write(uint32_t v) {
 return (uint8_t)(uc2_selected(v) &&
 ((logical_addr13_from_gpio(v) & 0x0Fu) == 0u));
}

static uint8_t uc2_pcr_write(uint32_t v) {
 return (uint8_t)(uc2_selected(v) &&
 ((logical_addr13_from_gpio(v) & 0x0Fu) == 0x0Cu));
}

static uint8_t ram_write_at(uint32_t v, uint32_t addr) {
 return (uint8_t)(!uc2_selected(v) &&
 (logical_addr13_from_gpio(v) == addr));
}

static uint8_t all_bus_gpios_are_inputs(ora_gpio_query_fn_t q) {
 static const uint8_t pins[] = {
 0,1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,26,27
 };
 uint32_t i;
 for (i = 0; i < (uint32_t)sizeof(pins); ++i) {
 ora_gpio_info_t info;
 info.size = sizeof(info);
 if (q(pins[i], &info) != ORA_RESULT_OK || info.is_output) return 0u;
 }
 return 1u;
}

/* Claim only the exclusive capture engines before touching a raw register.
 *
 * The sampled bus GPIOs are deliberately NOT claimed: Fire-24G has core-owned
 * selector/system inputs in the sampled range (GPIO0 is one), and V8.1 rightly
 * rejects an exclusive plugin claim for them.  This Monitor is physically
 * passive and never writes any sampled GPIO.  all_bus_gpios_are_inputs() is
 * the safety contract: startup still fails closed if any sampled line is an
 * output, while allowing core and Monitor to read the same input signal. */
static uint8_t claim_capture_resources(ora_lookup_fn_t lookup,
                                       volatile hud1541_mailbox_t *m) {
 ora_claim_dma_channel_fn_t claim_dma =
  (ora_claim_dma_channel_fn_t)lookup(ORA_ID_CLAIM_DMA_CHANNEL);
 ora_claim_pio_fn_t claim_pio = (ora_claim_pio_fn_t)lookup(ORA_ID_CLAIM_PIO);
 uint32_t dma = DMA_CH;
 uint32_t block = CAPTURE_PIO_BLOCK;
 uint32_t sm = CAPTURE_PIO_SM;
 uint32_t first = CAPTURE_PIO_FIRST_INSTR;

 if (claim_dma == 0 || claim_pio == 0) {
  m->pio_pc = 0xE0u; /* a required V8.1 claim API is unavailable */
  return 0u;
 }
 if (claim_dma(&dma, 0u) != ORA_RESULT_OK || dma != DMA_CH) {
  m->pio_pc = 0xE1u; /* DMA11 unavailable */
  return 0u;
 }
 if (claim_pio(&block, &sm, &first, CAPTURE_PIO_INSTR_COUNT,
               ORA_PIO_GPIOBASE_0, 0u) != ORA_RESULT_OK) {
  m->pio_pc = 0xE2u; /* PIO0/SM3/instruction 26..31 unavailable */
  return 0u;
 }
 if (block != CAPTURE_PIO_BLOCK || sm != CAPTURE_PIO_SM ||
     first != CAPTURE_PIO_FIRST_INSTR) {
  m->pio_pc = 0xE3u; /* impossible allocation mismatch */
  return 0u;
 }
 return 1u;
}

typedef enum {
 HUD1541_LED_BOOT,
 HUD1541_LED_OFF,
 HUD1541_LED_WAITING,
 HUD1541_LED_ACTIVE_D3,
 HUD1541_LED_ACTIVE_D2,
 HUD1541_LED_ACTIVE_D1,
 HUD1541_LED_ACTIVE_D0,
 HUD1541_LED_WARNING,
 HUD1541_LED_FAULT,
} hud1541_led_state_t;

/* Density colour is a live-reading indication only. Waiting, warning and
 * fault colours deliberately retain their operational meaning. */
static hud1541_led_state_t active_led_for_density(uint8_t density,
                                                   uint8_t valid) {
 if (!valid || density > 3u) return HUD1541_LED_ACTIVE_D3;
 switch (density) {
 case 3u: return HUD1541_LED_ACTIVE_D3; /* Tracks  1..17: yellow */
 case 2u: return HUD1541_LED_ACTIVE_D2; /* Tracks 18..24: blue   */
 case 1u: return HUD1541_LED_ACTIVE_D1; /* Tracks 25..30: orange */
 default: return HUD1541_LED_ACTIVE_D0; /* Tracks 31..35: white  */
 }
}

static void set_monitor_led(ora_led_set_fn_t led_set, hud1541_led_state_t state,
                            hud1541_led_state_t *previous) {
 ora_led_request_t req;
 uint8_t restore_status = *previous == HUD1541_LED_OFF && state != HUD1541_LED_OFF;
 if (led_set == 0 || state == *previous) return;
 req.size = (uint8_t)sizeof(req);
 req.led = ORA_LED_RGB;
 req.mode = ORA_LED_MODE_ON;
 req.brightness = 20u;
 req.red = 0u; req.green = 0u; req.blue = 0u;
 req.reserved0 = 0u; req.period_ms = 0u;
 req.reserved1[0] = 0u; req.reserved1[1] = 0u; req.hold_ms = 0u;
 if (state == HUD1541_LED_BOOT) {
  req.mode = ORA_LED_MODE_ON; req.brightness = 35u;
  req.red = 160u; req.green = 0u; req.blue = 255u;
 } else if (state == HUD1541_LED_OFF) {
  req.mode = ORA_LED_MODE_OFF;
 } else if (state == HUD1541_LED_WAITING) {
  /* A steady green is unambiguous idle/armed feedback.  It also cannot be
   * superseded by a previous bounded animation during early plugin start-up. */
  req.mode = ORA_LED_MODE_ON;
  req.red = 0u; req.green = 255u; req.blue = 45u;
 } else if (state == HUD1541_LED_ACTIVE_D3) {
  req.brightness = 30u; req.red = 255u; req.green = 210u; req.blue = 0u;
 } else if (state == HUD1541_LED_ACTIVE_D2) {
  req.brightness = 30u; req.red = 0u; req.green = 70u; req.blue = 255u;
 } else if (state == HUD1541_LED_ACTIVE_D1) {
  req.brightness = 30u; req.red = 255u; req.green = 95u; req.blue = 0u;
 } else if (state == HUD1541_LED_ACTIVE_D0) {
  req.brightness = 26u; req.red = 180u; req.green = 220u; req.blue = 255u;
 } else if (state == HUD1541_LED_WARNING) {
  req.mode = ORA_LED_MODE_BLINK; req.period_ms = 900u;
  req.red = 255u; req.green = 100u; req.blue = 0u;
 } else {
  req.mode = ORA_LED_MODE_BLINK; req.period_ms = 300u;
  req.red = 255u; req.green = 0u; req.blue = 0u;
 }
 /* Do not cache a state which the core declined to display. */
 if (led_set(&req) != ORA_RESULT_OK) return;
 /* Fire-24G exposes logical status and RGB channels on GPIO29. RGB-off by
  * itself leaves the status channel visibly driving that physical LED. */
 if (state == HUD1541_LED_OFF || restore_status) {
  req.led = ORA_LED_STATUS;
  req.mode = state == HUD1541_LED_OFF ? ORA_LED_MODE_OFF : ORA_LED_MODE_ON;
  req.brightness = 0u;
  req.red = 0u; req.green = 0u; req.blue = 0u;
  led_set(&req);
 }
 *previous = state;
}

/* Capture is continuous on a live 1541 bus; there is normally no interval in
 * which the raw-ring consumer is empty.  LED health therefore cannot be
 * refreshed only by the outer loop: after an overflow warning it could remain
 * amber forever while the consumer keeps pace with a never-ending producer.
 * This helper is also called from the bounded inner-loop cadence below. */
static void refresh_monitor_led(volatile hud1541_mailbox_t *m,
                                ora_led_set_fn_t led_set,
                                uint32_t *warning_until_us,
                                uint32_t *last_event_overflow,
                                hud1541_led_state_t *led_state,
                                uint32_t boot_until_us, uint8_t led_off) {
 if (m->event_overflow != *last_event_overflow) {
  *warning_until_us = TIMER0_RAWL + HUD1541_LED_WARNING_HOLD_US;
  *last_event_overflow = m->event_overflow;
 }

 if (led_off) {
  set_monitor_led(led_set, HUD1541_LED_OFF, led_state);
 } else if (m->flags & HUD1541_FLAG_UNSAFE_OUTPUT) {
  set_monitor_led(led_set, HUD1541_LED_FAULT, led_state);
 } else if ((int32_t)(boot_until_us - TIMER0_RAWL) > 0) {
  set_monitor_led(led_set, HUD1541_LED_BOOT, led_state);
 } else if ((int32_t)(*warning_until_us - TIMER0_RAWL) > 0) {
  set_monitor_led(led_set, HUD1541_LED_WARNING, led_state);
 } else if ((m->flags & HUD1541_FLAG_CAPTURE_VALID) && m->last_motor) {
  set_monitor_led(led_set,
   active_led_for_density(m->last_density, m->density_valid), led_state);
 } else {
  set_monitor_led(led_set, HUD1541_LED_WAITING, led_state);
 }
}

static void queue_event(volatile hud1541_mailbox_t *m, uint32_t word) {
 uint32_t head = m->event_head;
 uint32_t next = (head + 1u) % HUD1541_EVENT_WORDS;
 if (next == m->event_tail) {
 m->event_overflow++;
 m->flags |= HUD1541_FLAG_EVENT_OVERFLOW;
 return;
 }
 m->events[head] = word;
 __asm volatile("dmb sy" ::: "memory");
 m->event_head = next;
}

static void queue_motor(volatile hud1541_mailbox_t *m, uint8_t state) {
 queue_event(m, (HUD1541_EVENT_MOTOR << 28) | (uint32_t)(state & 1u));
}

static void queue_phase(volatile hud1541_mailbox_t *m,
 uint8_t oldp, uint8_t newp,
 uint8_t delta, uint8_t motor_snapshot) {
 queue_event(m,
 (HUD1541_EVENT_PHASE << 28) |
 ((uint32_t)(oldp & 3u) << 0) |
 ((uint32_t)(newp & 3u) << 2) |
 ((uint32_t)(delta & 3u) << 4) |
 ((uint32_t)(motor_snapshot & 1u) << 6));
}

static void queue_track_write(volatile hud1541_mailbox_t *m, uint8_t track) {
 queue_event(m, (HUD1541_EVENT_TRACK_WRITE << 28) | (uint32_t)track);
}

static void queue_write_protect(volatile hud1541_mailbox_t *m, uint8_t state) {
 queue_event(m, (HUD1541_EVENT_WRITE_PROTECT << 28) | (uint32_t)(state & 1u));
}

static void queue_density(volatile hud1541_mailbox_t *m, uint8_t density) {
 queue_event(m, (HUD1541_EVENT_DENSITY << 28) | (uint32_t)(density & 3u));
}

static void queue_write_gate(volatile hud1541_mailbox_t *m, uint8_t state) {
 queue_event(m, (HUD1541_EVENT_WRITE_GATE << 28) | (uint32_t)(state & 1u));
}

/* T0.0.11 diagnostic payloads:
 * HDRPHY type 6:
 *   27..21 decoded physical track
 *   20..16 decoded sector
 *   15..0  timestamp / 16 us
 *
 * RPM type 7:
 *   27..21 physical track
 *   20..16 inferred revolution count (1..4)
 *   15..0  RPM * 100
 */
static void queue_hdrphy_diag(volatile hud1541_mailbox_t *m,
 uint8_t track, uint8_t sector, uint16_t ticks16) {
 queue_event(m,
  (HUD1541_EVENT_HDRPHY_DIAG << 28) |
  ((uint32_t)(track & 0x7Fu) << 21) |
  ((uint32_t)(sector & 0x1Fu) << 16) |
  (uint32_t)ticks16);
}

/* Header metadata is emitted at a bounded rate to avoid competing with the
 * time-sensitive mechanism events. The payload is the exact bytes DOS wrote
 * after a confirmed physical GCR header: valid, checksum, ID1, ID2. */
static void queue_hdrmeta_diag(volatile hud1541_mailbox_t *m,
 uint8_t checksum, uint8_t id1, uint8_t id2, uint8_t valid) {
 queue_event(m,
  (HUD1541_EVENT_HDRMETA_DIAG << 28) |
  ((uint32_t)(valid & 1u) << 27) |
  ((uint32_t)checksum << 16) |
  ((uint32_t)id1 << 8) |
  (uint32_t)id2);
}

static void queue_rpm_diag(volatile hud1541_mailbox_t *m,
 uint8_t track, uint8_t revolutions, uint16_t rpm100) {
 queue_event(m,
  (HUD1541_EVENT_RPM_DIAG << 28) |
  ((uint32_t)(track & 0x7Fu) << 21) |
  ((uint32_t)(revolutions & 0x1Fu) << 16) |
  (uint32_t)rpm100);
}

/* Count one event per asserted SYNC pulse by watching the high->low edge and
 * return that edge to the RPM timer. The one-second count remains a useful
 * diagnostic; it is not used as a coarse tachometer. */
static uint8_t sync_poll(volatile hud1541_mailbox_t *m,
 uint8_t *last_level, uint32_t *count, uint32_t *next_report_us) {
 uint8_t level = (SIO_GPIO_IN & MASK_SYNC) ? 1u : 0u;
 uint8_t edge = (uint8_t)(*last_level && !level);
 uint32_t now = TIMER0_RAWL;

 if (edge) (*count)++;
 *last_level = level;

 if ((int32_t)(now - *next_report_us) >= 0) {
  if (telemetry_mask(m) & HUD1541_TELEM_SYNC) {
   uint32_t packed = (HUD1541_EVENT_SYNC_DIAG << 28) |
                     ((uint32_t)(level & 1u) << 27) |
                     (*count & 0x07FFFFFFu);
   queue_event(m, packed);
  }
  *count = 0u;
  *next_report_us = now + 1000000u;
 }
 return edge;
}

/* A formatted 1541 sector has one SYNC mark before its header and one before
 * its data. Therefore a standard DOS track has 42/38/36/34 raw SYNC edges
 * per revolution in zones 3/2/1/0 respectively. */
static uint8_t sync_edges_per_revolution(uint8_t track) {
 if (track >= 1u && track <= 17u) return 42u;
 if (track <= 24u) return 38u;
 if (track <= 30u) return 36u;
 if (track <= 35u) return 34u;
 return 0u;
}

/* Time an exact two-revolution physical edge window. A validated header
 * supplies the track zone only; it is deliberately not used as the timing
 * marker because DOS can skip or delay individual RAM header writes. */
static void sync_rpm_edge(volatile hud1541_mailbox_t *m, uint8_t edge,
 uint8_t motor_valid, uint8_t motor_on, uint8_t header_track_valid,
 uint8_t header_track, uint8_t stepping, uint16_t *intervals,
 uint32_t *start_us) {
 uint8_t edges_per_rev;
 uint16_t target_intervals;
 uint32_t now_us;

 if (!motor_valid || !motor_on || !header_track_valid || stepping) {
  *intervals = 0u;
  return;
 }
 if (!edge) return;

 edges_per_rev = sync_edges_per_revolution(header_track);
 if (!edges_per_rev) {
  *intervals = 0u;
  return;
 }
 target_intervals = (uint16_t)edges_per_rev * HDR_RPM_WINDOW_REVOLUTIONS;
 now_us = TIMER0_RAWL;
 if (*intervals == 0u) {
  *start_us = now_us;
  *intervals = 1u;
  return;
 }
 (*intervals)++;
 if (*intervals > target_intervals) {
  uint32_t elapsed_us = now_us - *start_us;
  uint32_t elapsed_100us;
  uint32_t rpm100;
  if (elapsed_us != 0u) {
   elapsed_100us = (elapsed_us + 50u) / 100u;
   rpm100 = ((60000000u * HDR_RPM_WINDOW_REVOLUTIONS) +
             (elapsed_100us / 2u)) / elapsed_100us;
   if (rpm100 >= HDR_RPM_MIN_100 && rpm100 <= HDR_RPM_MAX_100 &&
       (telemetry_mask(m) & HUD1541_TELEM_RPM)) {
    queue_rpm_diag(m, header_track, HDR_RPM_WINDOW_REVOLUTIONS,
     (uint16_t)rpm100);
   }
  }
  *intervals = 0u;
 }
}

static void publish_track_state(volatile hud1541_mailbox_t *m,
 uint8_t last_track, uint8_t track_write_valid,
 uint8_t pos2, uint8_t pos_valid,
 uint8_t stepping) {
 uint32_t s = (uint32_t)last_track;
 if (track_write_valid) s |= TRACK_STATE_TRACK_WRITE_VALID;
 s |= ((uint32_t)pos2 << TRACK_STATE_POS2_SHIFT) & TRACK_STATE_POS2_MASK;
 if (pos_valid) s |= TRACK_STATE_POS_VALID;
 if (stepping) s |= TRACK_STATE_STEPPING;
 m->track_state = s;
}

static uint8_t clamp_pos2_in(uint8_t pos2) {
 /* No artificial Track-41 ceiling. Only prevent uint8 wrap. */
 return (pos2 < 254u) ? (uint8_t)(pos2 + 1u) : pos2;
}

static uint8_t clamp_pos2_out(uint8_t pos2) {
 return (pos2 > 2u) ? (uint8_t)(pos2 - 1u) : 2u;
}

static uint32_t dma_update_producer(uint32_t produced_total,
 uint32_t *last_remaining) {
 uint32_t remaining = DMA11->transfer_count & DMA_COUNT_MASK;
 uint32_t advanced;
 if (remaining <= *last_remaining) {
 advanced = *last_remaining - remaining;
 } else {
 advanced = *last_remaining + (DMA_RELOAD - remaining);
 }
 *last_remaining = remaining;
 return produced_total + advanced;
}

static uint8_t update_ring_health(volatile hud1541_mailbox_t *m,
 uint32_t produced_total,
 uint32_t *consumer_total) {
 uint32_t available = produced_total - *consumer_total;
 if (available > HUD1541_RING_WORDS) {
 uint32_t lost = available - HUD1541_RING_WORDS;
 m->ring_overrun += lost;
 m->ring_pressure++;
 m->flags |= HUD1541_FLAG_RING_PRESSURE | HUD1541_FLAG_RING_OVERRUN;
 *consumer_total = produced_total - HUD1541_RING_WORDS;
 m->consumer_total = *consumer_total;
 return 1u;
 } else if (available >= 48u) {
 m->ring_pressure++;
 m->flags |= HUD1541_FLAG_RING_PRESSURE;
 }
 return 0u;
}

/* Startup failures deliberately use the existing DIAG P field (0xF1..0xF7).
 * That gives the USB side a deterministic answer while keeping the published
 * mailbox layout stable.  These values are never normal PIO program counters
 * (the capture program lives at 26..31). */
#define HUD1541_DIAG_BAD_PLUGIN_TYPE 0xF1u
#define HUD1541_DIAG_NO_GPIO_QUERY   0xF2u
#define HUD1541_DIAG_CLAIM_FAILED    0xF3u
#define HUD1541_DIAG_GPIO_NOT_INPUT  0xF4u
#define HUD1541_DIAG_PIO_SM_BUSY     0xF5u
#define HUD1541_DIAG_DMA_BUSY        0xF6u

static uint8_t pio_dma_init(volatile hud1541_mailbox_t *m) {
 /* A claimed SM3 or a live DMA11 means this is not the audited stock-V8
  * layout.  Fail closed rather than corrupt another function's capture. */
 if ((PIO_CTRL & (1u << 3)) != 0u) {
  m->flags |= HUD1541_FLAG_UNSAFE_OUTPUT;
  return 0u;
 }
 if ((DMA11->ctrl_trig & DMA_BUSY) != 0u) {
  m->pio_pc = HUD1541_DIAG_DMA_BUSY;
  m->flags |= HUD1541_FLAG_UNSAFE_OUTPUT;
  return 0u;
 }

 PIO_CTRL_CLR = (1u << 3);

 /*
 * PIO slots 26..31:
 * 26 WAIT 1 GPIO8
 * 27 JMP 28 [20]
 * 28 JMP PIN,31 GPIO9 R/W high = read, skip capture
 * 29 IN PINS,32 snapshot write cycle
 * 30 WAIT 0 GPIO8
 * 31 WAIT 0 GPIO8
 *
 * The sampling point is hardware-validated. Do not retune casually.
 */
 PIO_INSTR_MEM(26) = 0x2088u;
 PIO_INSTR_MEM(27) = 0x141Cu;
 PIO_INSTR_MEM(28) = 0x00DFu;
 PIO_INSTR_MEM(29) = 0x4000u;
 PIO_INSTR_MEM(30) = 0x2008u;
 PIO_INSTR_MEM(31) = 0x2008u;

 PIO_SM3_CLKDIV = (1u << 16);
 PIO_SM3_EXECCTRL = (31u << 12) | (26u << 7) | (9u << 24);
 PIO_SM3_SHIFTCTRL = (1u << 16);
 PIO_SM3_PINCTRL = 0u;
 PIO_SM3_INSTR = 0x001Au;

 DMA11->ctrl_trig = 0u;
 DMA11->read_addr = (uint32_t)(uintptr_t)&PIO_RXF3;
 DMA11->write_addr = (uint32_t)(uintptr_t)&m->ring[0];
 DMA11->transfer_count = DMA_TRIGGER_SELF_MAX;
 DMA11->ctrl_trig =
 DMA_EN | DMA_SIZE_32 | DMA_INCR_WRITE |
 DMA_RING_SIZE(8u) | DMA_RING_SEL |
 DMA_CHAIN_TO(DMA_CH) | DMA_TREQ(DREQ_PIO0_RX3) | DMA_IRQ_QUIET;

 /* Queue the jump while SM3 is disabled, then atomically enable only SM3.
  * This matches the V8.1 RP2350 PIO API.  The former code wrote an obsolete
  * CTRL restart bit and queued after enable; Fire-24G consequently left the
  * Monitor at PC 0 with an idle DMA channel. */
 PIO_SM3_INSTR = 0x001Au;
 PIO_CTRL_SET = (1u << 3);
 m->flags |= HUD1541_FLAG_PIO_RUNNING | HUD1541_FLAG_DMA_RUNNING;
 return 1u;
}

/* A quiet ring is not stopped hardware. Freeze passive capture while XIP is
 * offline for NV programming; the caller resumes it immediately afterwards. */
static uint8_t pio_dma_pause(volatile hud1541_mailbox_t *m) {
 PIO_CTRL_CLR = (1u << 3);
 DMA11->ctrl_trig = 0u;
 for (uint32_t i = 0u; i < 1000u; ++i) {
  if ((DMA11->ctrl_trig & DMA_BUSY) == 0u) {
   m->flags &= ~(HUD1541_FLAG_PIO_RUNNING | HUD1541_FLAG_DMA_RUNNING);
   return 1u;
  }
 }
 m->pio_pc = HUD1541_DIAG_DMA_BUSY;
 m->flags |= HUD1541_FLAG_UNSAFE_OUTPUT;
 return 0u;
}

static void decode_snapshot(volatile hud1541_mailbox_t *m,
 uint32_t snapshot,
 uint8_t *phase_valid, uint8_t *last_phase,
 uint8_t *motor_valid, uint8_t *last_motor,
 uint8_t *wp_valid, uint8_t *last_wp,
 uint8_t *density_valid, uint8_t *last_density,
 uint8_t *write_gate_valid, uint8_t *last_write_gate,
 uint8_t *track_write_valid, uint8_t *last_track_write,
 uint8_t *pos_valid, uint8_t *current_pos2,
 uint8_t *last_drvst, uint8_t *stepping,
 uint8_t *hdr_state, uint8_t *hdr_track, uint8_t *hdr_sector,
 uint8_t *hdr_checksum, uint8_t *hdr_id1, uint8_t *hdr_meta_divider,
 uint16_t *hdr_ticks16,
 uint8_t *rpm_marker_valid, uint8_t *rpm_marker_track,
 uint8_t *rpm_marker_sector, uint8_t *rpm_marker_id1,
 uint8_t *rpm_marker_id2, uint8_t *rpm_revolutions,
 uint32_t *rpm_start_us, uint32_t *rpm_last_us) {
 if (uc2_pcr_write(snapshot)) {
  uint8_t pcr = logical_data_from_gpio(snapshot);
  /* UC2 CB2 is the 1541 disk write gate. PCR CB2 manual-low (110b)
   * asserts it; manual-high (111b) releases it for normal reading. */
  uint8_t write_gate = (uint8_t)((pcr & 0xE0u) == 0xC0u);
  if (!*write_gate_valid || write_gate != *last_write_gate) {
   *write_gate_valid = 1u;
   *last_write_gate = write_gate;
   if (telemetry_mask(m) & HUD1541_TELEM_CORE) queue_write_gate(m, write_gate);
  }
 }
 if (uc2_orb_write(snapshot)) {
 uint8_t orb = logical_data_from_gpio(snapshot);
 uint8_t motor = (uint8_t)((orb >> 2) & 1u);
 uint8_t phase = (uint8_t)(orb & 3u);
 uint8_t density = (uint8_t)((orb >> 5) & 3u);

 m->last_motor = motor;
 m->last_density = density;

 if (!*motor_valid) {
 *motor_valid = 1u;
 *last_motor = motor;
 if (telemetry_mask(m) & HUD1541_TELEM_CORE) queue_motor(m, motor);
 } else if (motor != *last_motor) {
 *last_motor = motor;
 if (telemetry_mask(m) & HUD1541_TELEM_CORE) queue_motor(m, motor);

 if (!motor) {
  *rpm_marker_valid = 0u;
 }
 }

 if (!*phase_valid) {
 *phase_valid = 1u;
 *last_phase = phase;
 } else if (phase != *last_phase) {
 uint8_t oldp = *last_phase;
 uint8_t delta = (uint8_t)((phase - oldp) & 3u);

 *last_phase = phase;
 m->phase_event_count++;
 if (telemetry_mask(m) & HUD1541_TELEM_CORE) queue_phase(m, oldp, phase, delta, motor);

 /* Mirror the proven GUI half-step arithmetic in RP2350 SRAM.
  * This is background decode only; PIO/DMA acquisition is untouched.
  */
 if (*pos_valid) {
 if (delta == 1u) {
 *current_pos2 = clamp_pos2_in(*current_pos2);
 } else if (delta == 3u) {
 *current_pos2 = clamp_pos2_out(*current_pos2);
 }
 publish_track_state(m, *last_track_write, *track_write_valid,
 *current_pos2, *pos_valid, *stepping);
 }
 }

 /*
 * VIA2 PB5/PB6 are the drive's actual density-select outputs.
 * This reports the commanded hardware density, independent of track.
 */
 if (!*density_valid) {
 *density_valid = 1u;
 *last_density = density;
 m->density_valid = 1u;
 m->density_event_count++;
 if (telemetry_mask(m) & HUD1541_TELEM_CORE) queue_density(m, density);
 } else if (density != *last_density) {
 *last_density = density;
 m->density_event_count++;
 if (telemetry_mask(m) & HUD1541_TELEM_CORE) queue_density(m, density);
 }
 }
 /* Physical-header recognition for RPM.
  *
  * The 1541 ROM uses two different RAM write orders:
  *
  *   Actual decoded disk header:
  *     $18 -> $19 -> $1A -> $17 -> $16
  *
  *   DOS pre-search header image:
  *     $16 -> $17 -> $18 -> $19 -> $1A
  *
  * Only the first sequence represents a header actually decoded from
  * the rotating disk.  Timestamp $19, then wait for $1A/$17/$16 to
  * confirm the complete physical-header sequence before using it.
  */
 if (ram_write_at(snapshot, RAM_HDRTRK)) {
  *hdr_track = logical_data_from_gpio(snapshot);
  *hdr_state = 1u;
 } else if (ram_write_at(snapshot, RAM_HDRSEC)) {
  if (*hdr_state == 1u) {
   *hdr_sector = logical_data_from_gpio(snapshot);
   *hdr_ticks16 = (uint16_t)((TIMER0_RAWL >> 4) & 0xFFFFu);
   *hdr_state = 2u;
  } else {
   *hdr_state = 0u;
  }
 } else if (ram_write_at(snapshot, RAM_HDRCHK)) {
  if (*hdr_state == 2u) {
   *hdr_checksum = logical_data_from_gpio(snapshot);
   *hdr_state = 3u;
  } else {
   *hdr_state = 0u;
  }
 } else if (ram_write_at(snapshot, RAM_HDRID1)) {
  if (*hdr_state == 3u) {
   *hdr_id1 = logical_data_from_gpio(snapshot);
   *hdr_state = 4u;
  } else {
   *hdr_state = 0u;
  }
 } else if (ram_write_at(snapshot, RAM_HDRID2)) {
  if (*hdr_state == 4u &&
      *hdr_track >= 1u && *hdr_track <= 127u &&
      *hdr_sector <= 31u) {
   uint8_t sec = *hdr_sector;
   uint8_t id2 = logical_data_from_gpio(snapshot);
   uint8_t checksum_valid = (uint8_t)(
    *hdr_checksum == (uint8_t)(*hdr_track ^ sec ^ *hdr_id1 ^ id2));
   uint16_t now = *hdr_ticks16;

   /* Physical-header evidence drives Recent Sectors and Sector Coverage.
    * Export every valid header. The heavier metadata/RPM reports are still
    * sampled to keep the 32-event mailbox bounded during reads. */
   if (telemetry_mask(m) & HUD1541_TELEM_HEADERS)
    queue_hdrphy_diag(m, *hdr_track, sec, now);

   /* The capture core can observe roughly 50 physical headers each second,
    * while the CDC bridge serializes text through a 32-event mailbox. Report
    * one representative RPM/ID sample per eight headers. Capture and RPM
    * calculations still run on every header. */
   if (*hdr_meta_divider == 0u && (telemetry_mask(m) & HUD1541_TELEM_METADATA)) {
    queue_hdrmeta_diag(m, *hdr_checksum, *hdr_id1, id2, checksum_valid);
   }

   if (checksum_valid) {
    /* Header RAM writes identify a valid physical track and density zone.
     * sync_rpm_edge() below owns the timer and counts the raw SYNC waveform. */
    if (!*rpm_marker_valid || *rpm_marker_track != *hdr_track) {
     *rpm_marker_valid = 1u;
     *rpm_marker_track = *hdr_track;
     *rpm_marker_sector = sec;
     *rpm_marker_id1 = *hdr_id1;
     *rpm_marker_id2 = id2;
     *rpm_revolutions = 0u;
     *rpm_start_us = 0u;
     *rpm_last_us = 0u;
    }
   }
   *hdr_meta_divider = (uint8_t)((*hdr_meta_divider + 1u) & HDR_TELEM_DIVIDER_MASK);
  }
  *hdr_state = 0u;
 }


 if (ram_write_at(snapshot, RAM_DRVST)) {
 uint8_t data = logical_data_from_gpio(snapshot);
 uint8_t was_stepping = *stepping;
 *last_drvst = data;
 *stepping = (uint8_t)((data & 0x40u) ? 1u : 0u);
 m->last_drvst = data;

 /* Stock DOS writes destination DRVTRK before a seek, then clears DRVST
  * bit 6 after the head-settle phase. At that exact transition the cached
  * DRVTRK is again the physical full-track location.
  */
 if (was_stepping && !*stepping && *track_write_valid &&
 *last_track_write >= 1u && *last_track_write <= 127u) {
 uint32_t p2 = (uint32_t)(*last_track_write) * 2u;
 *current_pos2 = (uint8_t)((p2 > 254u) ? 254u : p2);
 *pos_valid = 1u;
 }

 publish_track_state(m, *last_track_write, *track_write_valid,
 *current_pos2, *pos_valid, *stepping);
 }

 if (ram_write_at(snapshot, RAM_DRVTRK)) {
 uint8_t data = logical_data_from_gpio(snapshot);

 /* DOS $0022 (DRVTRK) is the full-track target/current-track variable.
  * A disk read repeatedly writes the same target and transient zeroes here;
  * neither provides new position evidence.  Retain and report only changed,
  * valid targets so those trace records cannot flood the diagnostic queue. */
 if (data >= 1u && data <= 42u) {
  if (!*track_write_valid || data != *last_track_write) {
   if (telemetry_mask(m) & HUD1541_TELEM_CORE) queue_track_write(m, data);
  }
  *last_track_write = data;
  *track_write_valid = 1u;
 }

 if (data == 1u) {
 /* Strong HOME/bump anchor. Subsequent outward bump steps are clamped at 1.0. */
 *current_pos2 = 2u;
 *pos_valid = 1u;
 } else if (!*pos_valid && !*stepping && data >= 1u && data <= 127u) {
 /* If DOS supplies a track while not stepping, it is safe as the initial
  * full-track anchor. During normal seeks DRVST bit 6 is already set first.
  */
 uint32_t p2 = (uint32_t)data * 2u;
 *current_pos2 = (uint8_t)((p2 > 254u) ? 254u : p2);
 *pos_valid = 1u;
 }

 publish_track_state(m, *last_track_write, *track_write_valid,
 *current_pos2, *pos_valid, *stepping);
 }

 if (ram_write_at(snapshot, RAM_LWPT)) {
 uint8_t data = logical_data_from_gpio(snapshot);
 uint8_t wp = (uint8_t)((data & 0x10u) ? 1u : 0u);
 m->last_write_protect = wp;

 if (!*wp_valid) {
 *wp_valid = 1u;
 *last_wp = wp;
 m->write_protect_valid = 1u;
 if (telemetry_mask(m) & HUD1541_TELEM_CORE) queue_write_protect(m, wp);
 } else if (wp != *last_wp) {
 *last_wp = wp;
 if (telemetry_mask(m) & HUD1541_TELEM_CORE) queue_write_protect(m, wp);
 }
 }
}

void hud1541_probe_main(ora_lookup_fn_t lookup,
 ora_plugin_type_t type,
 const ora_entry_args_t *args) {
 ora_gpio_query_fn_t gpio_query;
 ora_led_set_fn_t led_set;
 volatile hud1541_mailbox_t *m = HUD1541_MAILBOX;
 uint32_t i;
 uint32_t consumer_total = 0u;
 uint32_t produced_total = 0u;
 uint32_t last_remaining = DMA_RELOAD;
 uint8_t phase_valid = 0u, motor_valid = 0u, wp_valid = 0u, density_valid = 0u, write_gate_valid = 0u;
 uint8_t last_phase = 0u, last_motor = 0u, last_wp = 0u, last_density = 2u, last_write_gate = 0u;
 uint8_t track_write_valid = 0u, last_track_write = 0u;
 uint8_t pos_valid = 0u, current_pos2 = 0u;
 uint8_t last_drvst = 0u, stepping = 0u;
 uint8_t sync_last_level = (SIO_GPIO_IN & MASK_SYNC) ? 1u : 0u;
 uint32_t sync_count = 0u;
 uint32_t sync_next_report_us = TIMER0_RAWL + 1000000u;
 uint32_t warning_until_us = 0u;
 uint32_t last_event_overflow = 0u;
 uint32_t boot_until_us = TIMER0_RAWL + HUD1541_LED_BOOT_HOLD_US;
 uint32_t last_led_request_seq = 0u;
 uint8_t led_off = __nv_storage_start[1] == HUD1541_LED_POLICY_OFF;
 uint8_t pending_led_policy = 0u;
 uint32_t pending_led_seq = 0u;

 /* Physical-header/RPM state is deliberately local to the plugin main
  * routine. Do not move these arrays to static/global storage: the USER
  * plugin has fixed RAM allocations and unallocated .bss state previously
  * corrupted the 1541HUD mailbox during T0.0.6.
  *
  * The history arrays are volatile intentionally. This is a freestanding
  * plugin with no libc; without volatile, GCC may optimize initialization
  * or clear loops into an unresolved memset() call, as seen during T0.0.11
  * development.
  */
 uint8_t hdr_state = 0u, hdr_track = 0u, hdr_sector = 0u;
 uint8_t hdr_checksum = 0u, hdr_id1 = 0u, hdr_meta_divider = 0u;
 uint16_t hdr_ticks16 = 0u;
 uint8_t rpm_marker_valid = 0u, rpm_marker_track = 0u;
 uint8_t rpm_marker_sector = 0u, rpm_marker_id1 = 0u, rpm_marker_id2 = 0u;
 uint8_t rpm_revolutions = 0u;
 uint32_t rpm_start_us = 0u, rpm_last_us = 0u;
 uint16_t rpm_sync_intervals = 0u;
 uint32_t rpm_sync_start_us = 0u;
 hud1541_led_state_t led_state = (hud1541_led_state_t)255;
 (void)args;
 gpio_query = (ora_gpio_query_fn_t)lookup(ORA_ID_GPIO_QUERY);
 led_set = (ora_led_set_fn_t)lookup(ORA_ID_LED_SET);

 m->magic = HUD1541_MAILBOX_MAGIC;
 m->version = HUD1541_MAILBOX_VERSION;
 m->flags = 0u;
 m->capture_count = 0u;
 m->consumed_count = 0u;
 m->event_head = 0u;
 m->event_tail = 0u;
 m->event_overflow = 0u;
 m->ring_pressure = 0u;
 m->led_request_seq = 0u;
 /* Publish the boot-restored policy so the USB transport can answer a
  * read-only HUDLED? query without writing flash or interrupting capture. */
 m->led_request_mode = led_off ? HUD1541_LED_POLICY_OFF : HUD1541_LED_POLICY_AUTO;
 m->last_motor = 0u;
 m->led_applied_seq = 0u;
 m->pio_pc = 0u;
 m->rx_level = 0u;
 m->dma_write_addr = 0u;
 m->consumer_index = 0u;
 m->producer_index = 0u;
 m->dma_busy = 0u;
 m->dma_trans_count = 0u;
 m->produced_total = 0u;
 m->consumer_total = 0u;
 m->ring_overrun = 0u;
 m->phase_event_count = 0u;
 m->last_write_protect = 0u;
 m->led_result = 0u;
 m->write_protect_valid = 0u;
 m->last_density = 2u; /* GUI/firmware startup assumption: Track 18 => D2 */
 m->density_event_count = 0u;
 m->density_valid = 0u;
 m->track_state = 0u;
 m->last_drvst = 0u;

 /* A persisted lights-off policy is dark from the first LED request. */
 set_monitor_led(led_set, led_off ? HUD1541_LED_OFF : HUD1541_LED_BOOT,
                 &led_state);

 for (i = 0u; i < HUD1541_EVENT_WORDS; ++i) m->events[i] = 0u;
 for (i = 0u; i < HUD1541_RING_WORDS; ++i) m->ring[i] = 0u;

 if (type != ORA_PLUGIN_TYPE_USER) {
 m->pio_pc = HUD1541_DIAG_BAD_PLUGIN_TYPE;
 m->flags = HUD1541_FLAG_UNSAFE_OUTPUT;
 set_monitor_led(led_set, HUD1541_LED_FAULT, &led_state);
 while (1) __asm volatile("wfi");
 }
 if (gpio_query == 0) {
 m->pio_pc = HUD1541_DIAG_NO_GPIO_QUERY;
 m->flags = HUD1541_FLAG_UNSAFE_OUTPUT;
 set_monitor_led(led_set, HUD1541_LED_FAULT, &led_state);
 while (1) __asm volatile("wfi");
 }
 if (!claim_capture_resources(lookup, m)) {
 if (m->pio_pc == 0u) m->pio_pc = HUD1541_DIAG_CLAIM_FAILED;
 m->flags = HUD1541_FLAG_UNSAFE_OUTPUT;
 set_monitor_led(led_set, HUD1541_LED_FAULT, &led_state);
 while (1) __asm volatile("wfi");
 }
 if (!all_bus_gpios_are_inputs(gpio_query)) {
 m->pio_pc = HUD1541_DIAG_GPIO_NOT_INPUT;
 m->flags = HUD1541_FLAG_UNSAFE_OUTPUT;
 set_monitor_led(led_set, HUD1541_LED_FAULT, &led_state);
 while (1) __asm volatile("wfi");
 }

 m->flags = HUD1541_FLAG_INPUTS_SAFE;
 if (!pio_dma_init(m)) {
  if (m->pio_pc == 0u) m->pio_pc = HUD1541_DIAG_PIO_SM_BUSY;
  set_monitor_led(led_set, HUD1541_LED_FAULT, &led_state);
  while (1) __asm volatile("wfi");
 }

 while (1) {
 sync_rpm_edge(m,
  sync_poll(m, &sync_last_level, &sync_count, &sync_next_report_us),
  motor_valid, last_motor, rpm_marker_valid, rpm_marker_track,
  stepping, &rpm_sync_intervals, &rpm_sync_start_us);
 produced_total = dma_update_producer(produced_total, &last_remaining);

 m->pio_pc = PIO_SM3_ADDR & 0x1Fu;
 m->rx_level = (PIO_FLEVEL >> 28) & 0xFu;
 m->dma_write_addr = DMA11->write_addr;
 m->consumer_index = consumer_total & (HUD1541_RING_WORDS - 1u);
 m->producer_index = produced_total & (HUD1541_RING_WORDS - 1u);
 m->dma_busy = (DMA11->ctrl_trig & DMA_BUSY) ? 1u : 0u;
 m->dma_trans_count = DMA11->transfer_count;
 m->produced_total = produced_total;
 m->consumer_total = consumer_total;

 if (update_ring_health(m, produced_total, &consumer_total)) {
  warning_until_us = TIMER0_RAWL + HUD1541_LED_WARNING_HOLD_US;
 }
 if (m->led_request_seq != last_led_request_seq) {
  led_off = m->led_request_mode == HUD1541_LED_POLICY_OFF;
  last_led_request_seq = m->led_request_seq;
  pending_led_policy = (uint8_t)m->led_request_mode;
  pending_led_seq = last_led_request_seq;
  __asm volatile("dmb sy" ::: "memory");
  m->led_result = 0u; /* accepted; persistence is intentionally deferred */
 }

 /* Flash must never be taken offline while the capture ring still contains a
  * live drive transaction.  The LED changes immediately, but acknowledgement
  * waits until the motor is off and the PIO/DMA consumer has caught up. */
 if (pending_led_policy != 0u && !m->last_motor && consumer_total == produced_total) {
  uint8_t saved = 0u;
  if (pio_dma_pause(m)) {
   saved = hud1541_save_led_policy(lookup, pending_led_policy);
   if (pio_dma_init(m)) {
    /* pio_dma_init resets the DMA write pointer, so reset the matching
     * software epoch rather than mixing old totals with a new ring. */
    produced_total = 0u;
    consumer_total = 0u;
    last_remaining = DMA_TRIGGER_SELF_MAX;
   } else {
    saved = 0u;
   }
  }
  m->led_result = saved ? HUD1541_LED_RESULT_OK : HUD1541_LED_RESULT_FAIL;
  m->led_applied_seq = pending_led_seq;
  pending_led_policy = 0u;
 }
 refresh_monitor_led(m, led_set, &warning_until_us, &last_event_overflow,
                     &led_state, boot_until_us, led_off);

 while (consumer_total != produced_total) {
 sync_rpm_edge(m,
  sync_poll(m, &sync_last_level, &sync_count, &sync_next_report_us),
  motor_valid, last_motor, rpm_marker_valid, rpm_marker_track,
  stepping, &rpm_sync_intervals, &rpm_sync_start_us);
 uint32_t idx = consumer_total & (HUD1541_RING_WORDS - 1u);
 uint32_t snapshot = m->ring[idx];

 consumer_total++;
 m->capture_count++;
 m->consumed_count++;
 m->consumer_total = consumer_total;
 m->flags |= HUD1541_FLAG_CAPTURE_VALID;

 decode_snapshot(m, snapshot,
 &phase_valid, &last_phase,
 &motor_valid, &last_motor,
 &wp_valid, &last_wp,
 &density_valid, &last_density,
 &write_gate_valid, &last_write_gate,
 &track_write_valid, &last_track_write,
 &pos_valid, &current_pos2,
 &last_drvst, &stepping,
 &hdr_state, &hdr_track, &hdr_sector,
 &hdr_checksum, &hdr_id1, &hdr_meta_divider, &hdr_ticks16,
 &rpm_marker_valid, &rpm_marker_track, &rpm_marker_sector, &rpm_marker_id1,
 &rpm_marker_id2, &rpm_revolutions, &rpm_start_us, &rpm_last_us);

 if (!rpm_marker_valid || !last_motor) rpm_sync_intervals = 0u;

 produced_total = dma_update_producer(produced_total, &last_remaining);
 m->produced_total = produced_total;
 if (update_ring_health(m, produced_total, &consumer_total)) {
  warning_until_us = TIMER0_RAWL + HUD1541_LED_WARNING_HOLD_US;
 }
 /* Do not wait for the producer to go idle: it never does during passive
  * capture.  Once amber's hold period expires, this returns the RGB LED to
  * green waiting or the live density colour even under sustained bus traffic. */
 if ((consumer_total & 0x3Fu) == 0u) {
  refresh_monitor_led(m, led_set, &warning_until_us, &last_event_overflow,
                      &led_state, boot_until_us, led_off);
 }
 }
 }
}
