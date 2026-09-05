/*
 * DriveHUD UB4 passive monitor V0.0.30
 *
 * Data path:
 * 1541 write cycles
 * -> PIO0 SM3 deterministic sampling
 * -> DMA11 circular ring
 * -> bus-event decoder
 * -> compact mailbox queue
 * -> USB CDC formatter
 * -> DriveHUD GUI
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
#include "drivehud_mailbox.h"

ORA_DEFINE_USER_PLUGIN(drivehud_probe_main, 0,0,30,0, 0,7,1);

#define RESETS_BASE 0x40020000u
#define RESET_RESET (*(volatile uint32_t *)(RESETS_BASE + 0x00u))
#define RESET_DONE (*(volatile uint32_t *)(RESETS_BASE + 0x08u))
#define RESET_DMA (1u << 2)
#define RESET_PIO0 (1u << 11)

#define PIO0_BASE 0x50200000u
#define PIO_CTRL (*(volatile uint32_t *)(PIO0_BASE + 0x000u))
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

#define MASK_CS1 (1u << 24)
#define MASK_nCS2 (1u << 25)

#define DMA_COUNT_MASK 0x0fffffffu
#define DMA_RELOAD 0x0fffffffu
#define DMA_TRIGGER_SELF_MAX 0x1fffffffu

#define RAM_LWPT 0x001Eu
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
 return (uint8_t)(((v & MASK_CS1) != 0u) && ((v & MASK_nCS2) == 0u));
}

static uint8_t uc2_orb_write(uint32_t v) {
 return (uint8_t)(uc2_selected(v) &&
 ((logical_addr13_from_gpio(v) & 0x0Fu) == 0u));
}

static uint8_t ram_write_at(uint32_t v, uint32_t addr) {
 return (uint8_t)(!uc2_selected(v) &&
 (logical_addr13_from_gpio(v) == addr));
}

static uint8_t all_bus_gpios_are_inputs(ora_gpio_query_fn_t q) {
 static const uint8_t pins[] = {
 0,1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
 };
 uint32_t i;
 for (i = 0; i < (uint32_t)sizeof(pins); ++i) {
 ora_gpio_info_t info;
 info.size = sizeof(info);
 if (q(pins[i], &info) != ORA_RESULT_OK || info.is_output) return 0u;
 }
 return 1u;
}

static void queue_event(volatile drivehud_mailbox_t *m, uint32_t word) {
 uint32_t head = m->event_head;
 uint32_t next = (head + 1u) % DRIVEHUD_EVENT_WORDS;
 if (next == m->event_tail) {
 m->event_overflow++;
 m->flags |= DRIVEHUD_FLAG_EVENT_OVERFLOW;
 return;
 }
 m->events[head] = word;
 __asm volatile("dmb sy" ::: "memory");
 m->event_head = next;
}

static void queue_motor(volatile drivehud_mailbox_t *m, uint8_t state) {
 queue_event(m, (DRIVEHUD_EVENT_MOTOR << 28) | (uint32_t)(state & 1u));
}

static void queue_phase(volatile drivehud_mailbox_t *m,
 uint8_t oldp, uint8_t newp,
 uint8_t delta, uint8_t motor_snapshot) {
 queue_event(m,
 (DRIVEHUD_EVENT_PHASE << 28) |
 ((uint32_t)(oldp & 3u) << 0) |
 ((uint32_t)(newp & 3u) << 2) |
 ((uint32_t)(delta & 3u) << 4) |
 ((uint32_t)(motor_snapshot & 1u) << 6));
}

static void queue_track_write(volatile drivehud_mailbox_t *m, uint8_t track) {
 queue_event(m, (DRIVEHUD_EVENT_TRACK_WRITE << 28) | (uint32_t)track);
}

static void queue_write_protect(volatile drivehud_mailbox_t *m, uint8_t state) {
 queue_event(m, (DRIVEHUD_EVENT_WRITE_PROTECT << 28) | (uint32_t)(state & 1u));
}

static void queue_density(volatile drivehud_mailbox_t *m, uint8_t density) {
 queue_event(m, (DRIVEHUD_EVENT_DENSITY << 28) | (uint32_t)(density & 3u));
}

static void publish_track_state(volatile drivehud_mailbox_t *m,
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

static void update_ring_health(volatile drivehud_mailbox_t *m,
 uint32_t produced_total,
 uint32_t *consumer_total) {
 uint32_t available = produced_total - *consumer_total;
 if (available > DRIVEHUD_RING_WORDS) {
 uint32_t lost = available - DRIVEHUD_RING_WORDS;
 m->ring_overrun += lost;
 m->ring_pressure++;
 m->flags |= DRIVEHUD_FLAG_RING_PRESSURE | DRIVEHUD_FLAG_RING_OVERRUN;
 *consumer_total = produced_total - DRIVEHUD_RING_WORDS;
 m->consumer_total = *consumer_total;
 } else if (available >= 48u) {
 m->ring_pressure++;
 m->flags |= DRIVEHUD_FLAG_RING_PRESSURE;
 }
}

static void pio_dma_init(volatile drivehud_mailbox_t *m) {
 RESET_RESET &= ~(RESET_PIO0 | RESET_DMA);
 while ((RESET_DONE & (RESET_PIO0 | RESET_DMA)) !=
 (RESET_PIO0 | RESET_DMA)) {}

 PIO_CTRL &= ~(1u << 3);
 PIO_CTRL |= (1u << (4 + 3));

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

 PIO_CTRL |= (1u << 3);
 m->flags |= DRIVEHUD_FLAG_PIO_RUNNING | DRIVEHUD_FLAG_DMA_RUNNING;
}

static void decode_snapshot(volatile drivehud_mailbox_t *m,
 uint32_t snapshot,
 uint8_t *phase_valid, uint8_t *last_phase,
 uint8_t *motor_valid, uint8_t *last_motor,
 uint8_t *wp_valid, uint8_t *last_wp,
 uint8_t *density_valid, uint8_t *last_density,
 uint8_t *track_write_valid, uint8_t *last_track_write,
 uint8_t *pos_valid, uint8_t *current_pos2,
 uint8_t *last_drvst, uint8_t *stepping) {
 if (uc2_orb_write(snapshot)) {
 uint8_t orb = logical_data_from_gpio(snapshot);
 uint8_t motor = (uint8_t)((orb >> 2) & 1u);
 uint8_t phase = (uint8_t)(orb & 3u);
 uint8_t density = (uint8_t)((orb >> 5) & 3u);

 m->last_orb = orb;
 m->last_phase = phase;
 m->last_motor = motor;
 m->last_density = density;

 if (!*motor_valid) {
 *motor_valid = 1u;
 *last_motor = motor;
 queue_motor(m, motor);
 } else if (motor != *last_motor) {
 *last_motor = motor;
 queue_motor(m, motor);
 }

 if (!*phase_valid) {
 *phase_valid = 1u;
 *last_phase = phase;
 } else if (phase != *last_phase) {
 uint8_t oldp = *last_phase;
 uint8_t delta = (uint8_t)((phase - oldp) & 3u);
 *last_phase = phase;
 m->phase_event_count++;
 queue_phase(m, oldp, phase, delta, motor);

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
 queue_density(m, density);
 } else if (density != *last_density) {
 *last_density = density;
 m->density_event_count++;
 queue_density(m, density);
 }
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
  * Keep the last value, and continue publishing the V28 event unchanged.
  */
 *last_track_write = data;
 *track_write_valid = 1u;
 queue_track_write(m, data);

 if (data == 1u) {
 /* Strong HOME/bump anchor. Subsequent outward bump steps are clamped at 1.0. */
 *current_pos2 = 2u;
 *pos_valid = 1u;
 m->home_count++;
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
 m->write_protect_event_count++;
 queue_write_protect(m, wp);
 } else if (wp != *last_wp) {
 *last_wp = wp;
 m->write_protect_event_count++;
 queue_write_protect(m, wp);
 }
 }
}

void drivehud_probe_main(ora_lookup_fn_t lookup,
 ora_plugin_type_t type,
 const ora_entry_args_t *args) {
 ora_gpio_query_fn_t gpio_query;
 volatile drivehud_mailbox_t *m = DRIVEHUD_MAILBOX;
 uint32_t i;
 uint32_t consumer_total = 0u;
 uint32_t produced_total = 0u;
 uint32_t last_remaining = DMA_RELOAD;
 uint8_t phase_valid = 0u, motor_valid = 0u, wp_valid = 0u, density_valid = 0u;
 uint8_t last_phase = 0u, last_motor = 0u, last_wp = 0u, last_density = 2u;
 uint8_t track_write_valid = 0u, last_track_write = 0u;
 uint8_t pos_valid = 0u, current_pos2 = 0u;
 uint8_t last_drvst = 0u, stepping = 0u;

 (void)args;
 gpio_query = (ora_gpio_query_fn_t)lookup(ORA_ID_GPIO_QUERY);

 m->magic = DRIVEHUD_MAILBOX_MAGIC;
 m->version = DRIVEHUD_MAILBOX_VERSION;
 m->flags = 0u;
 m->capture_count = 0u;
 m->consumed_count = 0u;
 m->event_head = 0u;
 m->event_tail = 0u;
 m->event_overflow = 0u;
 m->ring_pressure = 0u;
 m->last_orb = 0u;
 m->last_phase = 0u;
 m->last_motor = 0u;
 m->home_count = 0u;
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
 m->write_protect_event_count = 0u;
 m->write_protect_valid = 0u;
 m->last_density = 2u; /* GUI/firmware startup assumption: Track 18 => D2 */
 m->density_event_count = 0u;
 m->density_valid = 0u;
 m->track_state = 0u;
 m->last_drvst = 0u;

 for (i = 0u; i < DRIVEHUD_EVENT_WORDS; ++i) m->events[i] = 0u;
 for (i = 0u; i < DRIVEHUD_RING_WORDS; ++i) m->ring[i] = 0u;

 if (type != ORA_PLUGIN_TYPE_USER ||
 gpio_query == 0 ||
 !all_bus_gpios_are_inputs(gpio_query)) {
 m->flags = DRIVEHUD_FLAG_UNSAFE_OUTPUT;
 while (1) __asm volatile("wfi");
 }

 m->flags = DRIVEHUD_FLAG_INPUTS_SAFE;
 pio_dma_init(m);

 while (1) {
 produced_total = dma_update_producer(produced_total, &last_remaining);

 m->pio_pc = PIO_SM3_ADDR & 0x1Fu;
 m->rx_level = (PIO_FLEVEL >> 28) & 0xFu;
 m->dma_write_addr = DMA11->write_addr;
 m->consumer_index = consumer_total & (DRIVEHUD_RING_WORDS - 1u);
 m->producer_index = produced_total & (DRIVEHUD_RING_WORDS - 1u);
 m->dma_busy = (DMA11->ctrl_trig & DMA_BUSY) ? 1u : 0u;
 m->dma_trans_count = DMA11->transfer_count;
 m->produced_total = produced_total;
 m->consumer_total = consumer_total;

 update_ring_health(m, produced_total, &consumer_total);

 while (consumer_total != produced_total) {
 uint32_t idx = consumer_total & (DRIVEHUD_RING_WORDS - 1u);
 uint32_t snapshot = m->ring[idx];

 consumer_total++;
 m->capture_count++;
 m->consumed_count++;
 m->consumer_total = consumer_total;
 m->flags |= DRIVEHUD_FLAG_CAPTURE_VALID;

 decode_snapshot(m, snapshot,
 &phase_valid, &last_phase,
 &motor_valid, &last_motor,
 &wp_valid, &last_wp,
 &density_valid, &last_density,
 &track_write_valid, &last_track_write,
 &pos_valid, &current_pos2,
 &last_drvst, &stepping);

 produced_total = dma_update_producer(produced_total, &last_remaining);
 m->produced_total = produced_total;
 update_ring_health(m, produced_total, &consumer_total);
 }
 }
}
