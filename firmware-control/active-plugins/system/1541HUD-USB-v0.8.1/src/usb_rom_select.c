// 1541 OneROM USB Selector V0.8.0.01
// OneROM v0.8.1 USB system-plugin extension for Commodore 1541 UB3.
//
// Purpose:
//   Provide the CDC command surface used by the V0.8.0.01 USB selector. It enumerates
//   non-plugin OneROM flash slots and reads/updates the same persistent byte
//   used by the C64 1541 OneROM Selector.
//
// Compatibility contract taken from the project baselines:
//   Host Control NV sector: ORA_USER_PLUGIN_BASE + 64 KiB - 4 KiB
//   Persistent selected ROM: logical NV offset 0, one byte
//   Value: non-plugin flash-slot index.  0 is the universal bootloader;
//          selectable values are 1..min(non_plugin_count-1, 8).
//
// NV[0] is the production ROM-selection contract.  NV[2] is the configured
// boot IEC address; the USB selector reports and persists it with readback
// verification. NV[1] is the 1541HUD LED policy: A5=automatic, 5A=off.
// The NV location is intentionally isolated here so a future firmware/API-owned
// operation can replace the physical implementation without changing the GUI or
// the shared persistent-selection contract.

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "usb_plugin.h"
#include "usb_rom_select.h"
#include "tusb.h"
#include "system.h"

#define ROMSEL_VERSION             "V0.8.1.04"
#define ROMSEL_NV_SECTOR_SIZE      4096u
#define ROMSEL_USER_PLUGIN_SIZE    (64u * 1024u)
#define ROMSEL_NV_ADDR             (ORA_USER_PLUGIN_BASE + ROMSEL_USER_PLUGIN_SIZE - ROMSEL_NV_SECTOR_SIZE)
#define ROMSEL_NV_FLASH_OFFSET     (ROMSEL_NV_ADDR - ORA_FLASH_BASE_ADDR)
#define ROMSEL_SAVED_OFFSET        0u
#define ROMSEL_LED_POLICY_OFFSET   1u
#define ROMSEL_BOOT_IEC_OFFSET      2u
#define ROMSEL_LED_POLICY_AUTO     0xA5u
#define ROMSEL_LED_POLICY_OFF      0x5Au
#define ROMSEL_MAX_SELECTABLE      8u
#define ROMSEL_RX_MAX              63u

// RP2350 bootrom flash functions.  These signatures mirror the Host Control
// plugin's proven NV implementation.
typedef void (*flash_exit_xip_fn_t)(void);
typedef void (*flash_range_erase_fn_t)(uint32_t offs, uint32_t count, uint32_t block_size, uint8_t block_cmd);
typedef void (*flash_flush_cache_fn_t)(void);
typedef void (*flash_select_xip_read_mode_fn_t)(uint8_t mode, uint8_t clkdiv);
typedef void (*flash_range_program_fn_t)(uint32_t offs, const uint8_t *data, uint32_t count);
typedef int  (*flash_op_fn_t)(uint32_t flags, uint32_t addr, uint32_t size_bytes, uint8_t *buf);
typedef void (*connect_internal_flash_fn_t)(void);

#define FLASH_BLOCK_SIZE      65536u
#define FLASH_BLOCK_ERASE_CMD 0xD8u

/* RP2350 flash_op() flags: operation at bit 16, secure runtime address. */
#define ROMSEL_FLASH_ERASE_FLAGS   0x00000101u
#define ROMSEL_FLASH_PROGRAM_FLAGS 0x00010101u

static char s_rx[ROMSEL_RX_MAX + 1u];
static uint8_t s_rx_len;
static bool s_ready;

/*
 * A freshly programmed image has an erased NV sector (0xff).  Do not leave
 * boot behaviour to an accidental fallback: once the core APIs are available,
 * seed the documented first-boot defaults.  Existing valid user choices are
 * never rewritten.
 *
 * The writes run from romsel_task(), not romsel_init(), because a save needs
 * the core's exclusive-mode handshake to be fully live.  A failed handshake
 * is retried later rather than pounding flash or the other core every task.
 */
static bool s_default_slot_pending;
static bool s_default_iec_pending;
static uint16_t s_default_retry_delay;

// Write-protect override is deliberately transient: it is not backed by NV.
// This 1541 long-board/DD2 configuration uses SEL_C for the transistor and
// SEL_D for the divided physical write-protect sense input.  X1 and X2 are
// ROM-page selects, so either would corrupt ROM service if reused here.
// SEL_C and SEL_D are select-pin indexes 2 and 3 in OneROM metadata.
#define ROMSEL_WP_SEL_C_INDEX 2u
#define ROMSEL_WP_SEL_D_INDEX 3u
#define ROMSEL_RESET_SEL_B_INDEX 1u
// The 1 kOhm SEL_B-to-C46 resistor fitted on this 1541 discharges the 100 uF
// reset capacitor well below its roughly 2 V reset threshold. A 350 ms
// manual reset is long enough to be dependable without keeping the motor and
// red LED asserted for the previous bench-only ten-second diagnostic period.
#define ROMSEL_RESET_PULSE_MS 350u
// During a OneROM reboot the ROM-server GPIOs are unavailable briefly. Keep
// the drive in reset across that interval, but no longer than necessary.
#define ROMSEL_RESET_BOOT_HOLD_MS 350u
static uint8_t s_wp_gpio;
static bool s_wp_available;
static bool s_wp_on;
static uint8_t s_wp_sense_gpio;
static bool s_wp_sense_available;
static uint8_t s_reset_gpio;
static bool s_reset_available;
static bool s_reset_pending;
static bool s_reset_report_pending;
static uint32_t s_reset_release_at_ms;

// Fire-24G has an RGB LED.  It is driven by the V8.1 core, so this is not a
// third plugin or a raw GPIO driver.  Fire-24E simply refuses these requests.
static bool led_policy_off(void);

/* Send an RGB request without consulting the persisted HUD policy.  This is
 * used solely to enforce OFF itself; normal operational feedback goes through
 * rgb_set() below and is suppressed while the user has disabled the LEDs. */
static void rgb_set_raw(uint8_t mode, uint8_t red, uint8_t green, uint8_t blue,
                        uint8_t brightness, uint16_t period_ms, uint32_t hold_ms) {
    if (context.led_set == NULL) return;
    ora_led_request_t req = {0};
    req.size = sizeof(req);
    req.led = ORA_LED_RGB;
    req.mode = mode;
    req.red = red;
    req.green = green;
    req.blue = blue;
    req.brightness = brightness;
    req.period_ms = period_ms;
    req.hold_ms = hold_ms;
    (void)context.led_set(&req);
}

static void rgb_set(uint8_t mode, uint8_t red, uint8_t green, uint8_t blue,
                    uint8_t brightness, uint16_t period_ms, uint32_t hold_ms) {
    if (led_policy_off()) return;
    rgb_set_raw(mode, red, green, blue, brightness, period_ms, hold_ms);
}

static void rgb_slot(uint8_t slot) {
    static const uint8_t colours[9][3] = {
        {  0u,  64u, 180u }, // bootloader: blue
        {  0u, 160u, 220u }, // DOS 2.6: cyan
        {120u,  70u, 190u }, // 1541-II: purple
        {200u,   0u, 150u }, // JiffyDOS: magenta
        {220u, 100u,   0u }, // JiffyDOS parallel: amber
        { 90u, 180u,  30u }, // RAMBoard: lime
        {  0u, 150u, 120u }, // DolphinDOS: teal
        {220u, 220u, 220u }, // SpeedDOS: white
        {255u, 110u,  20u }, // SpeedDOS RAMBoard: orange
    };
    uint8_t index = (slot < 9u) ? slot : 0u;
    rgb_set(ORA_LED_MODE_ON, colours[index][0], colours[index][1],
            colours[index][2], 15u, 0u, 0u);
}

static void rgb_save_slot(uint8_t slot) {
    rgb_slot(slot);
    rgb_set(ORA_LED_MODE_BREATHE, 255u, 255u, 255u, 35u, 1000u, 350u);
}

static void rgb_save_iec(uint8_t address) {
    // Device 8 is two blue flashes; device 9 is three purple flashes.
    if (address == 8u) {
        rgb_set(ORA_LED_MODE_BLINK, 0u, 80u, 255u, 35u, 240u, 500u);
    } else {
        rgb_set(ORA_LED_MODE_BLINK, 150u, 0u, 255u, 35u, 240u, 740u);
    }
}

static ora_get_flash_slot_count_fn_t s_get_flash_slot_count;
static ora_get_flash_slot_info_fn_t  s_get_flash_slot_info;
static ora_get_flash_slot_ext_info_fn_t s_get_flash_slot_ext_info;
static ora_get_ram_slot_count_fn_t   s_get_ram_slot_count;
static ora_get_ram_slot_info_fn_t    s_get_ram_slot_info;
static ora_get_active_ram_slot_fn_t  s_get_active_ram_slot;
static ora_enter_exclusive_mode_fn_t s_enter_exclusive;
static ora_exit_exclusive_mode_fn_t  s_exit_exclusive;

/*
 * CDC writes are non-blocking.  A flush only starts an endpoint transfer; it
 * does not make newly freed FIFO space immediately available.  Retrying a
 * >64-byte selector reply synchronously therefore dropped its tail (often the
 * CR/LF) and made the desktop HUD wait for a reply that had almost arrived.
 *
 * Assemble one line here, then advance it over normal usb_main loop passes.
 * The selector processes the next command only after the preceding reply has
 * been accepted by TinyUSB, preserving the request/reply protocol.
 */
/* System plugins have a fixed 2 KiB shared static-RAM budget.  160 bytes
 * covers every selector record (including a full slot label) while leaving
 * the TinyUSB/picoboot state inside that budget. */
#define ROMSEL_CDC_REPLY_CAPACITY 160u
static char s_cdc_reply[ROMSEL_CDC_REPLY_CAPACITY];
static uint16_t s_cdc_reply_length;
static uint16_t s_cdc_reply_sent;

static void cdc_drain_reply(void) {
    if (s_cdc_reply_sent >= s_cdc_reply_length) {
        s_cdc_reply_sent = 0u;
        s_cdc_reply_length = 0u;
        return;
    }

    uint32_t available = tud_cdc_n_write_available(0);
    if (available == 0u) return;
    uint32_t remaining = (uint32_t)(s_cdc_reply_length - s_cdc_reply_sent);
    uint32_t count = remaining < available ? remaining : available;
    uint32_t written = tud_cdc_n_write(0, s_cdc_reply + s_cdc_reply_sent, count);
    if (written == 0u) return;
    s_cdc_reply_sent = (uint16_t)(s_cdc_reply_sent + written);
    (void)tud_cdc_n_write_flush(0);
    if (s_cdc_reply_sent >= s_cdc_reply_length) {
        s_cdc_reply_sent = 0u;
        s_cdc_reply_length = 0u;
    }
}

static void cdc_write(const char *s) {
    if (s == NULL) return;
    while (*s != '\0' && s_cdc_reply_length < (ROMSEL_CDC_REPLY_CAPACITY - 1u)) {
        s_cdc_reply[s_cdc_reply_length++] = *s++;
    }
    s_cdc_reply[s_cdc_reply_length] = '\0';
}

static void cdc_write_u32(uint32_t v) {
    char tmp[11];
    uint8_t n = 0;
    if (v == 0u) {
        cdc_write("0");
        return;
    }
    while (v != 0u && n < sizeof(tmp)) {
        tmp[n++] = (char)('0' + (v % 10u));
        v /= 10u;
    }
    while (n != 0u) {
        char text[2] = { tmp[--n], '\0' };
        cdc_write(text);
    }
}

static void reply_end(void) {
    cdc_write("\r\n");
    cdc_drain_reply();
}

static uint8_t selectable_max(void) {
    if (s_get_flash_slot_count == NULL) return 0u;
    uint8_t count = s_get_flash_slot_count(ORA_FLASH_SLOT_FLAG_EXCLUDE_PLUGINS);
    if (count <= 1u) return 0u;
    uint8_t max = (uint8_t)(count - 1u);
    return (max > ROMSEL_MAX_SELECTABLE) ? ROMSEL_MAX_SELECTABLE : max;
}

static uint8_t saved_slot(void) {
    const volatile uint8_t *nv = (const volatile uint8_t *)(uintptr_t)ROMSEL_NV_ADDR;
    return nv[ROMSEL_SAVED_OFFSET];
}

static uint8_t boot_iec_address(void) {
    const volatile uint8_t *nv = (const volatile uint8_t *)(uintptr_t)ROMSEL_NV_ADDR;
    return nv[ROMSEL_BOOT_IEC_OFFSET];
}

static bool boot_iec_valid(uint8_t address) {
    return address >= 8u && address <= 9u;
}

// SEL_B reaches C46/CR7 through the verified 1k resistor.  Assert only LOW, then release to
// INPUT; this discharges the capacitor and lets the original 1541 one-shot
// reset UC2/UC4.  Never drive the node HIGH.
static bool reset_discover_sel_b(void) {
    ora_get_metadata_uint_at_fn_t get_metadata_uint_at =
        context.ora_lookup_fn(ORA_ID_GET_METADATA_UINT_AT);
    if (get_metadata_uint_at == NULL || context.gpio_set == NULL ||
        context.gpio_query == NULL || context.get_plugin_uptime_ms == NULL ||
        context.num_gpios == 0u) return false;

    uint32_t gpio = 0u;
    if (get_metadata_uint_at(ORA_METADATA_KEY_GPIO_SEL,
                             ROMSEL_RESET_SEL_B_INDEX, &gpio) != ORA_RESULT_OK ||
        gpio == 0xffu || gpio >= context.num_gpios) return false;
    s_reset_gpio = (uint8_t)gpio;
    return true;
}

static bool reset_sel_b_is_free(void) {
    if (!s_reset_available || context.gpio_query == NULL) return false;
    ora_gpio_info_t info = { .size = sizeof(info) };
    return context.gpio_query(s_reset_gpio, &info) == ORA_RESULT_OK &&
           info.use == ORA_GPIO_USE_FREE;
}

static bool reset_release(void) {
    return reset_sel_b_is_free() &&
           context.gpio_set(s_reset_gpio, ORA_GPIO_STATE_INPUT, 0u) == ORA_RESULT_OK;
}

static bool reset_assert_for(uint32_t hold_ms, bool report_completion) {
    if (!reset_sel_b_is_free() || context.get_plugin_uptime_ms == NULL ||
        context.gpio_set(s_reset_gpio, ORA_GPIO_STATE_LOW, 0u) != ORA_RESULT_OK) return false;
    s_reset_release_at_ms = context.get_plugin_uptime_ms() + hold_ms;
    s_reset_pending = true;
    s_reset_report_pending = report_completion;
    if (report_completion) {
        rgb_set(ORA_LED_MODE_BREATHE, 255u, 100u, 0u, 30u, 1000u, 7000u);
    }
    return true;
}

static bool reset_start_pulse(void) {
    return reset_assert_for(ROMSEL_RESET_PULSE_MS, true);
}

static void reset_service(void) {
    if (!s_reset_pending || context.get_plugin_uptime_ms == NULL) return;
    if ((int32_t)(context.get_plugin_uptime_ms() - s_reset_release_at_ms) < 0) return;
    s_reset_pending = false;
    bool report_completion = s_reset_report_pending;
    s_reset_report_pending = false;
    bool released = reset_release();
    if (report_completion) {
        if (released) {
            cdc_write("$ROMTEST,RESET,PULSE_RELEASED"); reply_end();
        } else {
            cdc_write("$ROMTEST,RESET,RELEASE_FAILED"); reply_end();
        }
    }
}

// Discover SEL_C from the board metadata.  The Fire 24E maps it to GPIO26,
// but do not hard-code that electrical detail into the control protocol.
static bool wp_discover_sel_c(void) {
    ora_get_metadata_uint_at_fn_t get_metadata_uint_at =
        context.ora_lookup_fn(ORA_ID_GET_METADATA_UINT_AT);
    if (get_metadata_uint_at == NULL || context.gpio_set == NULL ||
        context.gpio_query == NULL || context.num_gpios == 0u) {
        return false;
    }

    uint32_t gpio = 0u;
    if (get_metadata_uint_at(ORA_METADATA_KEY_GPIO_SEL,
                             ROMSEL_WP_SEL_C_INDEX, &gpio) != ORA_RESULT_OK ||
        gpio == 0xffu || gpio >= context.num_gpios) {
        return false;
    }
    s_wp_gpio = (uint8_t)gpio;
    return true;
}

// Do not use FORCE for WP override.  SEL_C is reserved in the image config;
// if firmware reports it in use, refuse rather than steal an active signal.
static bool wp_sel_c_is_free(void) {
    if (!s_wp_available || context.gpio_query == NULL) return false;
    ora_gpio_info_t info = { .size = sizeof(info) };
    return context.gpio_query(s_wp_gpio, &info) == ORA_RESULT_OK &&
           info.use == ORA_GPIO_USE_FREE;
}

static bool wp_read_state(bool *on_out) {
    if (on_out == NULL || !wp_sel_c_is_free()) return false;
    ora_gpio_info_t info = { .size = sizeof(info) };
    if (context.gpio_query(s_wp_gpio, &info) != ORA_RESULT_OK ||
        info.is_output == 0u) {
        return false;
    }
    *on_out = info.level != 0u;
    return true;
}

// A LOW output is the normal safe state.  It leaves the physical disk notch
// sensor in control.  HIGH only turns on the external NPN transistor, which
// pulls P6 pin 12 low to request the existing 1541 WP circuitry's writable
// condition.
static bool wp_set_state(bool on) {
    if (!wp_sel_c_is_free() || context.gpio_set == NULL) return false;
    uint8_t state = on ? ORA_GPIO_STATE_HIGH : ORA_GPIO_STATE_LOW;
    if (context.gpio_set(s_wp_gpio, state, 0u) != ORA_RESULT_OK) {
        s_wp_on = false;
        return false;
    }

    bool verify = false;
    if (!wp_read_state(&verify) || verify != on) {
        if (on) {
            (void)context.gpio_set(s_wp_gpio, ORA_GPIO_STATE_LOW, 0u);
        }
        s_wp_on = false;
        return false;
    }
    s_wp_on = on;
    return true;
}

// Leave SEL_C electrically untouched at plugin start.  This is deliberately
// lazy because the pin is connected to external write-protect hardware; the
// first explicit ROMWP command owns the first GPIO state transition.
static bool wp_prepare(void) {
    if (!s_wp_available) s_wp_available = wp_discover_sel_c();
    return s_wp_available;
}

// SEL_D/GPIO27 is connected only through the external 22k/22k divider from
// 1541 P6 pin 12.  It must always remain an input; GPIO27 is 3.3 V-only on
// RP2350A and may also be used for SWD when a debugger is attached.
static bool wp_sense_discover_sel_d(void) {
    ora_get_metadata_uint_at_fn_t get_metadata_uint_at =
        context.ora_lookup_fn(ORA_ID_GET_METADATA_UINT_AT);
    if (get_metadata_uint_at == NULL || context.gpio_set == NULL ||
        context.gpio_query == NULL || context.num_gpios == 0u) {
        return false;
    }

    uint32_t gpio = 0u;
    if (get_metadata_uint_at(ORA_METADATA_KEY_GPIO_SEL,
                             ROMSEL_WP_SEL_D_INDEX, &gpio) != ORA_RESULT_OK ||
        gpio == 0xffu || gpio >= context.num_gpios) {
        return false;
    }
    s_wp_sense_gpio = (uint8_t)gpio;
    return true;
}

static bool wp_sense_sel_d_is_free(void) {
    if (!s_wp_sense_available || context.gpio_query == NULL) return false;
    ora_gpio_info_t info = { .size = sizeof(info) };
    return context.gpio_query(s_wp_sense_gpio, &info) == ORA_RESULT_OK &&
           info.use == ORA_GPIO_USE_FREE;
}

static bool wp_sense_prepare(void) {
    if (!s_wp_sense_available) s_wp_sense_available = wp_sense_discover_sel_d();
    if (!wp_sense_sel_d_is_free()) return false;
    // Release the output driver before every read.  This does not enable a
    // pull or otherwise alter the divider's measured voltage.
    return context.gpio_set(s_wp_sense_gpio, ORA_GPIO_STATE_INPUT, 0u) ==
           ORA_RESULT_OK;
}

// P6 pin 12 is high when the physical sensor reports write-protected and low
// when it reports writable.  SEL_C override ON deliberately forces this node
// low, so no physical claim is made while override is active.
static bool wp_sense_read(bool *protected_out) {
    if (protected_out == NULL || !wp_sense_prepare()) return false;
    ora_gpio_info_t info = { .size = sizeof(info) };
    if (context.gpio_query(s_wp_sense_gpio, &info) != ORA_RESULT_OK ||
        info.is_output != 0u) {
        return false;
    }
    *protected_out = info.level != 0u;
    return true;
}

static void wp_write_sense_fields(void) {
    bool protected = false;
    bool sense_ok = wp_sense_read(&protected);
    cdc_write(",SENSE_AVAILABLE="); cdc_write(sense_ok ? "1" : "0");
    cdc_write(",PHYSICAL=");
    if (!sense_ok) {
        cdc_write("UNAVAILABLE");
    } else if (s_wp_on) {
        // The override transistor owns the same node being sampled.
        cdc_write("MASKED");
    } else {
        cdc_write(protected ? "PROTECTED" : "WRITABLE");
    }
}

static void cmd_wp_get(void) {
    bool on = false;
    bool ok = wp_prepare() && wp_read_state(&on);
    if (ok) s_wp_on = on;
    cdc_write("$ROMTEST,WP,STATE=");
    cdc_write((ok && on) ? "ON" : "OFF");
    cdc_write(",AVAILABLE="); cdc_write(ok ? "1" : "0");
    if (!ok) cdc_write(",ERROR=SEL_C_UNAVAILABLE_OR_IN_USE");
    wp_write_sense_fields();
    reply_end();
}

static void cmd_wp_set(bool on) {
    bool ok = wp_prepare() && wp_set_state(on);
    cdc_write("$ROMTEST,WP,"); cdc_write(ok ? "OK" : "FAIL");
    cdc_write(",STATE="); cdc_write((ok && s_wp_on) ? "ON" : "OFF");
    cdc_write(",VERIFY="); cdc_write((ok && s_wp_on) ? "ON" : "OFF");
    cdc_write(",AVAILABLE="); cdc_write(ok ? "1" : "0");
    if (!ok) cdc_write(",ERROR=SEL_C_UNAVAILABLE_OR_IN_USE");
    wp_write_sense_fields();
    reply_end();
}

static bool saved_valid(uint8_t slot) {
    uint8_t max = selectable_max();
    return max != 0u && slot >= 1u && slot <= max;
}

static void *boot_lookup(char a, char b) {
    uint32_t code = ((uint32_t)(uint8_t)b << 8u) | (uint32_t)(uint8_t)a;
    return ORA_BOOTROM_LOOKUP(code, ORA_BOOTROM_FLAG_ARM_SEC);
}

#if defined(ORA_HOST_TEST)
static inline uint32_t irq_disable_local(void) { return 0u; }
static inline void irq_restore_local(uint32_t primask) { (void)primask; }
#else
static inline uint32_t irq_disable_local(void) {
    uint32_t primask;
    __asm volatile ("mrs %0, primask \n\t"
                    "cpsid i"
                    : "=r" (primask) :: "memory");
    return primask;
}

static inline void irq_restore_local(uint32_t primask) {
    __asm volatile ("msr primask, %0" :: "r" (primask) : "memory");
}
#endif

// Runs from system-plugin SRAM while XIP is unavailable.  Keep this routine
// self-contained: no globals, literals requiring flash, or external calls other
// than the function pointers passed in.
#if defined(ROMSEL_USE_RANGE_FLASH_API)
ORA_SECTION(".ramfunc") __attribute__((noinline))
static int romsel_flash_commit_ram(
    flash_exit_xip_fn_t exit_xip,
    flash_range_erase_fn_t range_erase,
    flash_range_program_fn_t range_program,
    flash_flush_cache_fn_t flush,
    flash_select_xip_read_mode_fn_t select_xip,
    uint32_t flash_offset,
    const uint8_t *data,
    uint32_t size,
    uint8_t clkdiv,
    int *erase_rc_out,
    int *program_rc_out
) {
#if defined(ORA_HOST_TEST)
    exit_xip();
    range_erase(flash_offset, size, FLASH_BLOCK_SIZE, FLASH_BLOCK_ERASE_CMD);
    *erase_rc_out = 0;
    range_program(flash_offset, data, size);
    *program_rc_out = 0;
    select_xip(3u, clkdiv);
    flush();
    return 0;
#else
    uint32_t primask;
    __asm volatile ("mrs %0, primask \n\t"
                    "cpsid i"
                    : "=r" (primask) :: "memory");
    exit_xip();
    range_erase(flash_offset, size, FLASH_BLOCK_SIZE, FLASH_BLOCK_ERASE_CMD);
    *erase_rc_out = 0;
    range_program(flash_offset, data, size);
    *program_rc_out = 0;
    select_xip(3u, clkdiv);
    flush();
    __asm volatile ("msr primask, %0" :: "r" (primask) : "memory");
    return 0;
#endif
}
#endif /* ROMSEL_USE_RANGE_FLASH_API */

/*
 * Production selector backend.  RP2350 Boot ROM flash_op() proved reliable
 * in the installed-drive persistence test.  The alternate range_erase() /
 * range_program() implementation remains available only when explicitly
 * requested with ROMSEL_USE_RANGE_FLASH_API.
 */
#if !defined(ROMSEL_USE_RANGE_FLASH_API)
ORA_SECTION(".ramfunc") __attribute__((noinline))
static int romsel_flash_op_commit_ram(
    flash_exit_xip_fn_t exit_xip,
    flash_op_fn_t flash_op,
    flash_flush_cache_fn_t flush,
    flash_select_xip_read_mode_fn_t select_xip,
    uint32_t flash_runtime_addr,
    const uint8_t *data,
    uint32_t size,
    uint8_t clkdiv,
    int *erase_rc_out,
    int *program_rc_out
) {
#if defined(ORA_HOST_TEST)
    exit_xip();
    int erase_rc = flash_op(ROMSEL_FLASH_ERASE_FLAGS,
                            flash_runtime_addr, size, NULL);
    *erase_rc_out = erase_rc;
    int program_rc = -1;
    if (erase_rc == 0) {
        program_rc = flash_op(ROMSEL_FLASH_PROGRAM_FLAGS,
                              flash_runtime_addr, size, (uint8_t *)data);
    }
    *program_rc_out = program_rc;
    select_xip(3u, clkdiv);
    flush();
    return (erase_rc != 0) ? erase_rc : program_rc;
#else
    uint32_t primask;
    __asm volatile ("mrs %0, primask \n\t"
                    "cpsid i"
                    : "=r" (primask) :: "memory");
    exit_xip();
    int erase_rc = flash_op(ROMSEL_FLASH_ERASE_FLAGS,
                            flash_runtime_addr, size, NULL);
    *erase_rc_out = erase_rc;
    int program_rc = -1;
    if (erase_rc == 0) {
        program_rc = flash_op(ROMSEL_FLASH_PROGRAM_FLAGS,
                              flash_runtime_addr, size, (uint8_t *)data);
    }
    *program_rc_out = program_rc;
    select_xip(3u, clkdiv);
    flush();
    __asm volatile ("msr primask, %0" :: "r" (primask) : "memory");
    return (erase_rc != 0) ? erase_rc : program_rc;
#endif
}

#endif /* !ROMSEL_USE_RANGE_FLASH_API */

static bool choose_staging(uint32_t *base_out) {
    if (s_get_ram_slot_count == NULL || s_get_ram_slot_info == NULL ||
        s_get_active_ram_slot == NULL || base_out == NULL) {
        return false;
    }
    uint8_t active = 0xFFu;
    if (s_get_active_ram_slot(&active) != ORA_RESULT_OK) return false;
    uint8_t count = s_get_ram_slot_count();
    // Prefer the highest non-active slot, keeping the low slots least disturbed.
    for (uint32_t i = count; i != 0u; i--) {
        uint8_t slot = (uint8_t)(i - 1u);
        if (slot == active) continue;
        uint32_t base = 0u, size = 0u;
        if (s_get_ram_slot_info(slot, &base, &size, NULL) == ORA_RESULT_OK &&
            size >= ROMSEL_NV_SECTOR_SIZE) {
            *base_out = base;
            return true;
        }
    }
    return false;
}

// Retain flash diagnostics in RAM for a later ROMDIAG request.
static int s_last_erase_rc = -999;
static int s_last_program_rc = -999;
static int s_last_flash_rc = -999;
static uint8_t s_last_flash_verify = 0xff;
static bool s_last_flash_diag_valid = false;
/* v1.0.0 path diagnostic; flash behavior is unchanged.
 * 0 none, 1 entered, 2 prereq fail, 3 already saved, 4 staging fail,
 * 5 staged, 6 bootrom lookup fail, 7 bootrom OK, 8 exclusive fail,
 * 9 exclusive entered, 10 calling flash, 11 flash returned,
 * 12 exclusive exited, 13 diagnostics/verify reached.
 */
static volatile uint32_t s_last_path_stage = 0u;

/*
 * Test the dangerous XIP-off/XIP-on transition without erasing or programming
 * the selector sector.  This is deliberately separate from write_nv_byte():
 * a normal save cannot report while XIP is unavailable, so a stalled command
 * tells us only that something in the whole critical section wedged.  ROMXIP
 * narrows that down without changing NV0/NV2 or the selected ROM.
 *
 * It runs from system-plugin SRAM for the same reason as the real flash
 * commit: after exit_xip(), neither code nor literals may be fetched from
 * external flash until select_xip_read_mode() has restored XIP.
 */
ORA_SECTION(".ramfunc") __attribute__((noinline))
static int romsel_xip_roundtrip_ram(
    flash_exit_xip_fn_t exit_xip,
    flash_flush_cache_fn_t flush,
    flash_select_xip_read_mode_fn_t select_xip,
    uint8_t clkdiv
) {
#if defined(ORA_HOST_TEST)
    exit_xip();
    select_xip(3u, clkdiv);
    flush();
    return 0;
#else
    uint32_t primask;
    __asm volatile ("mrs %0, primask \n\t"
                    "cpsid i"
                    : "=r" (primask) :: "memory");
    exit_xip();
    select_xip(3u, clkdiv);
    flush();
    __asm volatile ("msr primask, %0" :: "r" (primask) : "memory");
    return 0;
#endif
}

static bool xip_roundtrip_test(void) {
    s_last_path_stage = 1u;
    s_last_flash_diag_valid = false;
    if (s_enter_exclusive == NULL || s_exit_exclusive == NULL) {
        s_last_path_stage = 2u;
        return false;
    }

    connect_internal_flash_fn_t connect_internal_flash =
        (connect_internal_flash_fn_t)boot_lookup('I', 'F');
    flash_exit_xip_fn_t exit_xip = (flash_exit_xip_fn_t)boot_lookup('E', 'X');
    flash_flush_cache_fn_t flush = (flash_flush_cache_fn_t)boot_lookup('F', 'C');
    flash_select_xip_read_mode_fn_t select_xip =
        (flash_select_xip_read_mode_fn_t)boot_lookup('X', 'M');
    if (connect_internal_flash == NULL || exit_xip == NULL || flush == NULL ||
        select_xip == NULL) {
        s_last_path_stage = 6u;
        return false;
    }
    s_last_path_stage = 7u;

    if (s_enter_exclusive() != ORA_RESULT_OK) {
        s_last_path_stage = 8u;
        return false;
    }
    s_last_path_stage = 9u;

    uint32_t caller_primask = irq_disable_local();
    connect_internal_flash();
    uint8_t clkdiv = ORA_XIP_CLKDIV();
    s_last_path_stage = 10u;
    int result = romsel_xip_roundtrip_ram(exit_xip, flush, select_xip, clkdiv);
    s_last_path_stage = 11u;

    irq_restore_local(caller_primask);
    s_exit_exclusive();
    s_last_path_stage = 12u;
    return result == 0;
}

static bool write_nv_byte(uint32_t offset, uint8_t value) {
    s_last_path_stage = 1u;
    s_last_flash_diag_valid = false;
    if (offset >= ROMSEL_NV_SECTOR_SIZE || s_enter_exclusive == NULL || s_exit_exclusive == NULL) {
        s_last_path_stage = 2u;
        return false;
    }
    const volatile uint8_t *nv = (const volatile uint8_t *)(uintptr_t)ROMSEL_NV_ADDR;
    if (nv[offset] == value) {
        s_last_path_stage = 3u;
        return true;
    }

    uint32_t stage_base = 0u;
    if (!choose_staging(&stage_base)) {
        s_last_path_stage = 4u;
        return false;
    }
    volatile uint8_t *stage = ORA_SRAM_PTR(stage_base);
    for (uint32_t i = 0u; i < ROMSEL_NV_SECTOR_SIZE; i++) stage[i] = nv[i];
    stage[offset] = value;
    s_last_path_stage = 5u;

    connect_internal_flash_fn_t connect_internal_flash =
        (connect_internal_flash_fn_t)boot_lookup('I', 'F');
    flash_exit_xip_fn_t exit_xip = (flash_exit_xip_fn_t)boot_lookup('E', 'X');
#if !defined(ROMSEL_USE_RANGE_FLASH_API)
    flash_op_fn_t flash_op = (flash_op_fn_t)boot_lookup('F', 'O');
#else
    flash_range_erase_fn_t range_erase =
        (flash_range_erase_fn_t)boot_lookup('R', 'E');
    flash_range_program_fn_t range_program =
        (flash_range_program_fn_t)boot_lookup('R', 'P');
#endif
    flash_flush_cache_fn_t flush = (flash_flush_cache_fn_t)boot_lookup('F', 'C');
    flash_select_xip_read_mode_fn_t select_xip =
        (flash_select_xip_read_mode_fn_t)boot_lookup('X', 'M');
    if (connect_internal_flash == NULL || exit_xip == NULL || flush == NULL ||
        select_xip == NULL
#if !defined(ROMSEL_USE_RANGE_FLASH_API)
        || flash_op == NULL
#else
        || range_erase == NULL || range_program == NULL
#endif
    ) {
        s_last_path_stage = 6u;
        return false;
    }
    s_last_path_stage = 7u;

    if (s_enter_exclusive() != ORA_RESULT_OK) {
        s_last_path_stage = 8u;
        return false;
    }
    s_last_path_stage = 9u;

#if defined(ROMSEL_EXCLUSIVE_TEST)
    /*
     * Hardware diagnostic only: exercise the exact cross-core pause/resume
     * handshake used by a save, but do not touch XIP or flash.  A prompt
     * FAIL response means the handshake returned; a UI timeout means the
     * other core never acknowledged exclusive mode.
     */
    s_exit_exclusive();
    s_last_path_stage = 12u;
    return false;
#endif

    // Mask before connect_internal_flash(), just like Host Control.  The RAM
    // routine also saves/restores PRIMASK around the XIP-off interval so this
    // remains safe if the caller was already masked.
    uint32_t caller_primask = irq_disable_local();
    connect_internal_flash();
    uint8_t clkdiv = ORA_XIP_CLKDIV();
    int erase_rc = -1;
    int program_rc = -1;
    s_last_path_stage = 10u;
#if !defined(ROMSEL_USE_RANGE_FLASH_API)
    int flash_rc = romsel_flash_op_commit_ram(
        exit_xip, flash_op, flush, select_xip,
        ROMSEL_NV_ADDR,
        (const uint8_t *)stage,
        ROMSEL_NV_SECTOR_SIZE,
        clkdiv,
        &erase_rc,
        &program_rc);
#else
    int flash_rc = romsel_flash_commit_ram(
        exit_xip, range_erase, range_program, flush, select_xip,
        ROMSEL_NV_FLASH_OFFSET,
        (const uint8_t *)stage,
        ROMSEL_NV_SECTOR_SIZE,
        clkdiv,
        &erase_rc,
        &program_rc);
#endif
    s_last_path_stage = 11u;

    irq_restore_local(caller_primask);
    s_exit_exclusive();
    s_last_path_stage = 12u;

    uint8_t verify = nv[offset];

    s_last_erase_rc = erase_rc;
    s_last_program_rc = program_rc;
    s_last_flash_rc = flash_rc;
    s_last_flash_verify = verify;
    s_last_flash_diag_valid = true;
    s_last_path_stage = 13u;

    if (flash_rc != 0) {
        return false;
    }

    return verify == value;
}

static bool write_saved_slot(uint8_t slot) {
    return saved_valid(slot) && write_nv_byte(ROMSEL_SAVED_OFFSET, slot);
}

static bool write_boot_iec_address(uint8_t address) {
    return boot_iec_valid(address) && write_nv_byte(ROMSEL_BOOT_IEC_OFFSET, address);
}

static bool led_policy_off(void) {
    return ((const volatile uint8_t *)(uintptr_t)ROMSEL_NV_ADDR)[ROMSEL_LED_POLICY_OFFSET] == ROMSEL_LED_POLICY_OFF;
}

static void status_led_set(uint8_t mode) {
    if (context.led_set == NULL) return;
    ora_led_request_t request = {0};
    request.size = sizeof(request);
    request.led = ORA_LED_STATUS;
    request.mode = mode;
    (void)context.led_set(&request);
}

/* NV[1] is intentionally a paired front-panel policy.  Fire-24G maps the
 * status and RGB channels to the same physical LED GPIO, so changing only one
 * channel produces misleading blended colours.  AUTO restores the ordinary
 * per-ROM colour and status indicator; OFF immediately darkens both. */
static void apply_led_policy(void) {
    if (led_policy_off()) {
        rgb_set_raw(ORA_LED_MODE_OFF, 0u, 0u, 0u, 0u, 0u, 0u);
        status_led_set(ORA_LED_MODE_OFF);
        return;
    }
    status_led_set(ORA_LED_MODE_ON);
    rgb_slot(saved_valid(saved_slot()) ? saved_slot() : 0u);
}

static bool set_led_policy(bool off) {
    uint8_t value = off ? ROMSEL_LED_POLICY_OFF : ROMSEL_LED_POLICY_AUTO;
    if (!write_nv_byte(ROMSEL_LED_POLICY_OFFSET, value)) return false;
    apply_led_policy();
    return true;
}


static void seed_first_boot_defaults(void) {
    if (!s_ready) return;

    if (s_default_retry_delay != 0u) {
        s_default_retry_delay--;
        return;
    }

    if (s_default_slot_pending) {
        /* Slot zero is the bootloader/common image.  Slot one is the first
         * selectable ROM.  A one-slot composition has no valid selection to
         * persist, so leave NV0 alone in that deliberately reduced layout. */
        if (selectable_max() == 0u || write_saved_slot(1u)) {
            s_default_slot_pending = false;
            rgb_slot(saved_slot());
        } else {
            s_default_retry_delay = 500u;
            return;
        }
    }

    if (s_default_iec_pending) {
        if (write_boot_iec_address(8u)) {
            s_default_iec_pending = false;
        } else {
            s_default_retry_delay = 500u;
        }
    }
}

static void cmd_info(void) {
    cdc_write("$ROMTEST,INFO,VERSION="); cdc_write(ROMSEL_VERSION);
    cdc_write(",READY="); cdc_write(s_ready ? "1" : "0");
    cdc_write(",NV_ROM_OFFSET=0,NV_BOOT_IEC_OFFSET=2,MAX="); cdc_write_u32(selectable_max());
    reply_end();
}

static void cmd_get(void) {
    uint8_t slot = saved_slot();
    uint8_t address = boot_iec_address();
    cdc_write("$ROMTEST,SAVED,SLOT="); cdc_write_u32(slot);
    cdc_write(",VALID="); cdc_write(saved_valid(slot) ? "1" : "0");
    cdc_write(",BOOT_IEC="); cdc_write_u32(address);
    cdc_write(",BOOT_IEC_VALID="); cdc_write(boot_iec_valid(address) ? "1" : "0");
    reply_end();
}

static void cmd_reset(void) {
    bool ok = reset_start_pulse();
    cdc_write("$ROMTEST,RESET,"); cdc_write(ok ? "PULSE_STARTED" : "FAIL");
    cdc_write(",GPIO="); cdc_write_u32(s_reset_available ? s_reset_gpio : 0u);
    cdc_write(",HOLD_MS="); cdc_write_u32(ROMSEL_RESET_PULSE_MS);
    if (!ok) cdc_write(",ERROR=SEL_B_UNAVAILABLE_OR_IN_USE");
    reply_end();
}

static void cmd_slot(uint8_t i) {
    if (!s_ready) {
        cdc_write("$ROMTEST,ERROR,API_NOT_READY"); reply_end(); return;
    }
    uint8_t count = s_get_flash_slot_count(ORA_FLASH_SLOT_FLAG_EXCLUDE_PLUGINS);
    if (i >= count) {
        cdc_write("$ROMTEST,ERROR,INVALID_INDEX,INDEX="); cdc_write_u32(i);
        cdc_write(",COUNT="); cdc_write_u32(count); reply_end(); return;
    }

    const char *name = NULL;
    uint32_t rom_type = 0u;
    uint8_t rom_count = 0u;
    (void)s_get_flash_slot_info(i, ORA_FLASH_SLOT_FLAG_EXCLUDE_PLUGINS,
                                &name, &rom_type, &rom_count);
    /*
     * Keep the complete SLOT record below the TinyUSB CDC TX FIFO pressure
     * point observed during hardware validation. The longer ROM_TYPE/ROM_COUNT
     * prefix caused label truncation; those fields are diagnostic-only and the
     * GUI does not use them.
     */
    cdc_write("$ROMTEST,SLOT,INDEX="); cdc_write_u32(i);
    cdc_write(",SELECTABLE="); cdc_write((i >= 1u && i <= selectable_max()) ? "1" : "0");
    cdc_write(",NAME=");
    /*
     * A UB3/74LS138 drive group has three members in this exact order:
     * C000 on native /CS, A000 on X1, E000 on X2. Older two-page groups
     * have C000 then E000. The basic API only returns image 0, which is
     * normally the shared C000 file. Inspect every member and prefer an
     * explicit non-path label, so ROMSLOT reports "DOLPHINDOS 2.0" rather
     * than an arbitrary page filename in either layout.
     */
    const char *slot_label = name;
    if (rom_count > 1u && s_get_flash_slot_ext_info != NULL) {
        for (uint8_t image = 0u; image < rom_count; image++) {
            const char *candidate = NULL;
            if (s_get_flash_slot_ext_info(i, image,
                                          ORA_FLASH_SLOT_FLAG_EXCLUDE_PLUGINS,
                                          NULL, &candidate, NULL, NULL) != ORA_RESULT_OK ||
                candidate == NULL || candidate[0] == '\0') {
                continue;
            }
            bool path_like = false;
            for (uint8_t ch = 0u; candidate[ch] != '\0'; ch++) {
                if (candidate[ch] == '/' || candidate[ch] == '\\' || candidate[ch] == ':') {
                    path_like = true;
                    break;
                }
            }
            if (!path_like) {
                slot_label = candidate;
                break;
            }
        }
    }
    cdc_write(slot_label != NULL ? slot_label : "UNNAMED");
    reply_end();
}

static void cmd_list(void) {
    if (!s_ready) {
        cdc_write("$ROMTEST,ERROR,API_NOT_READY"); reply_end(); return;
    }
    // Return only the list header. Sending all eight slot records synchronously
    // from the CDC RX callback can overflow TinyUSB's transmit FIFO and
    // splice/drop response bytes.
    // The host now requests one slot at a time with ROMSLOT=<index>.
    uint8_t count = s_get_flash_slot_count(ORA_FLASH_SLOT_FLAG_EXCLUDE_PLUGINS);
    cdc_write("$ROMTEST,LIST,COUNT="); cdc_write_u32(count);
    cdc_write(",SELECTABLE_MAX="); cdc_write_u32(selectable_max()); reply_end();
}

static bool parse_slot_arg(const char *s, uint8_t *slot_out) {
    if (s == NULL || slot_out == NULL || *s == '\0') return false;
    uint32_t v = 0u;
    while (*s != '\0') {
        if (*s < '0' || *s > '9') return false;
        v = v * 10u + (uint32_t)(*s - '0');
        if (v > 255u) return false;
        s++;
    }
    *slot_out = (uint8_t)v;
    return true;
}

static void process_line(char *line) {
    while (*line == ' ' || *line == '\t') line++;
    if (strcmp(line, "ROMTEST?") == 0 || strcmp(line, "ROMINFO") == 0) {
        cmd_info();
    } else if (strcmp(line, "ROMLIST") == 0) {
        cmd_list();
    } else if (strncmp(line, "ROMSLOT=", 8u) == 0) {
        uint8_t slot = 0u;
        if (!parse_slot_arg(line + 8, &slot)) {
            cdc_write("$ROMTEST,ERROR,BAD_SLOT_ARGUMENT"); reply_end(); return;
        }
        cmd_slot(slot);
    } else if (strcmp(line, "ROMGET") == 0) {
        cmd_get();
    } else if (strcmp(line, "ROMWP?") == 0) {
        cmd_wp_get();
    } else if (strcmp(line, "ROMWP=ON") == 0) {
        cmd_wp_set(true);
    } else if (strcmp(line, "ROMWP=OFF") == 0) {
        cmd_wp_set(false);
    } else if (strcmp(line, "ROMLED?") == 0) {
        cdc_write("$ROMTEST,LED,OK,MODE="); cdc_write(led_policy_off() ? "OFF" : "AUTO"); reply_end();
    } else if (strcmp(line, "ROMLED=AUTO") == 0 || strcmp(line, "ROMLED=OFF") == 0) {
        bool off = strcmp(line, "ROMLED=OFF") == 0;
        bool ok = set_led_policy(off);
        cdc_write("$ROMTEST,LED,"); cdc_write(ok ? "OK,MODE=" : "FAIL,MODE="); cdc_write(off ? "OFF" : "AUTO");
        cdc_write(",VERIFY="); cdc_write(led_policy_off() == off ? "1" : "0"); reply_end();
    } else if (strncmp(line, "ROMSET=", 7u) == 0) {
        uint8_t slot = 0u;
        if (!parse_slot_arg(line + 7, &slot)) {
            cdc_write("$ROMTEST,SET,FAIL,BAD_ARGUMENT"); reply_end(); return;
        }
        if (!saved_valid(slot)) {
            cdc_write("$ROMTEST,SET,FAIL,INVALID_SLOT,SLOT="); cdc_write_u32(slot);
            cdc_write(",MAX="); cdc_write_u32(selectable_max()); reply_end(); return;
        }
        bool ok = write_saved_slot(slot);
        if (ok) rgb_save_slot(slot);
        uint8_t address = boot_iec_address();
        cdc_write("$ROMTEST,SET,"); cdc_write(ok ? "OK" : "FAIL");
        cdc_write(",SLOT="); cdc_write_u32(slot);
        cdc_write(",VERIFY="); cdc_write_u32(saved_slot());
        cdc_write(",BOOT_IEC="); cdc_write_u32(address);
        cdc_write(",BOOT_IEC_VALID="); cdc_write(boot_iec_valid(address) ? "1" : "0");
        reply_end();
    } else if (strncmp(line, "ROMIEC=", 7u) == 0) {
        uint8_t address = 0u;
        if (!parse_slot_arg(line + 7, &address)) {
            cdc_write("$ROMTEST,IEC,FAIL,BAD_ARGUMENT"); reply_end(); return;
        }
        if (!boot_iec_valid(address)) {
            cdc_write("$ROMTEST,IEC,FAIL,INVALID_ADDRESS,ADDRESS="); cdc_write_u32(address);
            cdc_write(",MIN=8,MAX=11"); reply_end(); return;
        }
        bool ok = write_boot_iec_address(address);
        if (ok) rgb_save_iec(address);
        uint8_t verify = boot_iec_address();
        cdc_write("$ROMTEST,IEC,"); cdc_write(ok ? "OK" : "FAIL");
        cdc_write(",ADDRESS="); cdc_write_u32(address);
        cdc_write(",VERIFY="); cdc_write_u32(verify);
        cdc_write(",SAVED_SLOT="); cdc_write_u32(saved_slot());
        cdc_write(",VALID="); cdc_write(boot_iec_valid(verify) ? "1" : "0");
        reply_end();
    } else if (strcmp(line, "ROMRESET") == 0) {
        cmd_reset();
    } else if (strcmp(line, "ROMDIAGS") == 0) {
        cdc_write("$ROMTEST,S=");
        cdc_write_u32((uint32_t)s_last_path_stage);
        reply_end();
        return;
    } else if (strcmp(line, "ROMDIAGE") == 0) {
        if (!s_last_flash_diag_valid) {
            cdc_write("$ROMTEST,E,NONE");
        } else {
            cdc_write("$ROMTEST,E=");
            cdc_write_u32((uint32_t)s_last_erase_rc);
        }
        reply_end();
    } else if (strcmp(line, "ROMDIAGP") == 0) {
        if (!s_last_flash_diag_valid) {
            cdc_write("$ROMTEST,P,NONE");
        } else {
            cdc_write("$ROMTEST,P=");
            cdc_write_u32((uint32_t)s_last_program_rc);
        }
        reply_end();
    } else if (strcmp(line, "ROMDIAGF") == 0) {
        if (!s_last_flash_diag_valid) {
            cdc_write("$ROMTEST,F,NONE");
        } else {
            cdc_write("$ROMTEST,F=");
            cdc_write_u32((uint32_t)s_last_flash_rc);
        }
        reply_end();
    } else if (strcmp(line, "ROMDIAGV") == 0) {
        if (!s_last_flash_diag_valid) {
            cdc_write("$ROMTEST,V,NONE");
        } else {
            cdc_write("$ROMTEST,V=");
            cdc_write_u32((uint32_t)s_last_flash_verify);
        }
        reply_end();
    } else if (strcmp(line, "ROMXIP") == 0) {
        bool ok = xip_roundtrip_test();
        cdc_write("$ROMTEST,XIP,");
        cdc_write(ok ? "OK" : "FAIL");
        cdc_write(",STAGE="); cdc_write_u32(s_last_path_stage);
        reply_end();
    } else if (strcmp(line, "ROMHELP") == 0) {
        cdc_write("$ROMTEST,HELP,ROMTEST? ROMLIST ROMSLOT ROMGET ROMSET ROMIEC(8..9) ROMRESET ROMXIP ROMWP? ROMWP=ON ROMWP=OFF ROMDIAGS ROMDIAGx");
        reply_end();
    } else if (*line != '\0') {
        cdc_write("$ROMTEST,ERROR,UNKNOWN_COMMAND");
        reply_end();
    }
}

void romsel_init(void) {
    s_get_flash_slot_count = context.ora_lookup_fn(ORA_ID_GET_FLASH_SLOT_COUNT);
    s_get_flash_slot_info  = context.ora_lookup_fn(ORA_ID_GET_FLASH_SLOT_INFO);
    s_get_flash_slot_ext_info = context.ora_lookup_fn(ORA_ID_GET_FLASH_SLOT_EXT_INFO);
    s_get_ram_slot_count   = context.ora_lookup_fn(ORA_ID_GET_RAM_SLOT_COUNT);
    s_get_ram_slot_info    = context.ora_lookup_fn(ORA_ID_GET_RAM_SLOT_INFO);
    s_get_active_ram_slot  = context.ora_lookup_fn(ORA_ID_GET_ACTIVE_RAM_SLOT);
    s_enter_exclusive      = context.ora_lookup_fn(ORA_ID_ENTER_EXCLUSIVE_MODE);
    s_exit_exclusive       = context.ora_lookup_fn(ORA_ID_EXIT_EXCLUSIVE_MODE);
    s_ready = s_get_flash_slot_count != NULL && s_get_flash_slot_info != NULL &&
              s_get_ram_slot_count != NULL && s_get_ram_slot_info != NULL &&
              s_get_active_ram_slot != NULL && s_enter_exclusive != NULL &&
              s_exit_exclusive != NULL;
    s_rx_len = 0u;
    s_default_slot_pending = s_ready && !saved_valid(saved_slot());
    s_default_iec_pending = s_ready && !boot_iec_valid(boot_iec_address());
    s_default_retry_delay = 0u;

    // SEL_C drives the external NPN write-protect override.  The safe state
    // is an actively driven LOW output: merely recording s_wp_on = false
    // leaves the pin high-impedance and can let the NPN base float after a
    // power cycle.  Make the physical state agree with the HUD state before
    // accepting any commands.
    s_wp_on = false;
    s_wp_available = false;
    s_wp_sense_available = false;
    s_reset_available = reset_discover_sel_b();
    s_reset_pending = false;
    if (wp_prepare()) {
        (void)wp_set_state(false);
    }
    // Prepare the sense pin early, but failure is non-fatal: SEL_C override
    // remains usable on builds without the optional SEL_D divider wired.
    (void)wp_sense_prepare();
    // The core has already asserted reserved SEL_B at the earliest GPIO
    // setup. Continue holding it long enough to discharge the C46 reset cap
    // before releasing the 1541 into a fully serving OneROM. This completion
    // is deliberately silent: it is a boot interlock, not a HUD request.
    if (s_reset_available) (void)reset_assert_for(ROMSEL_RESET_BOOT_HOLD_MS, false);
    apply_led_policy();
}

void romsel_cdc_rx(const uint8_t *buf, uint32_t count) {
    for (uint32_t i = 0u; i < count; i++) {
        char c = (char)buf[i];
        if (c == '\r' || c == '\n') {
            if (s_rx_len != 0u) {
                s_rx[s_rx_len] = '\0';
                process_line(s_rx);
                s_rx_len = 0u;
            }
            continue;
        }
        if (s_rx_len < ROMSEL_RX_MAX) {
            s_rx[s_rx_len++] = c;
        } else {
            s_rx_len = 0u;
            cdc_write("$ROMTEST,ERROR,LINE_TOO_LONG"); reply_end();
        }
    }
}


void romsel_task(void) {
    seed_first_boot_defaults();
    reset_service();

    /* Complete a long CDC reply before accepting another command. */
    cdc_drain_reply();
    if (s_cdc_reply_length != 0u) return;

    uint8_t buf[64];
    if (tud_cdc_n_available(0) == 0u) return;
    uint32_t count = tud_cdc_n_read(0, buf, sizeof(buf));
    if (count != 0u) romsel_cdc_rx(buf, count);
}
