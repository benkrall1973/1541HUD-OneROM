// DriveHUD UB4 passive probe v0.0.1
// Test 1: prove USER plugin startup/coexistence with stock USB SYSTEM plugin.
#include "plugin.h"

ORA_DEFINE_USER_PLUGIN(
    drivehud_probe_main,
    0, 0, 1, 0,
    0, 7, 1
);

static void delay(volatile uint32_t n) {
    while (n--) {
        __asm volatile ("nop");
    }
}

void drivehud_probe_main(
    ora_lookup_fn_t ora_lookup_fn,
    ora_plugin_type_t plugin_type,
    const ora_entry_args_t *entry_args
) {
    (void)entry_args;

    ora_set_status_led_fn_t set_led =
        (ora_set_status_led_fn_t)ora_lookup_fn(ORA_ID_SET_STATUS_LED);

    if (set_led == 0 || plugin_type != ORA_PLUGIN_TYPE_USER) {
        while (1) { __asm volatile ("wfi"); }
    }

    // Signature: two short flashes, long gap, repeat.
    while (1) {
        set_led(1); delay(900000);
        set_led(0); delay(900000);
        set_led(1); delay(900000);
        set_led(0); delay(6000000);
    }
}
