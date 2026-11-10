#ifndef HUD_TEST_USB_PLUGIN_H
#define HUD_TEST_USB_PLUGIN_H
/* Host boundary only: keep OneROM API types real; stub USB transport I/O. */
#include <stdbool.h>
#include <stdint.h>
#include "plugin.h"
typedef struct {
 ora_lookup_fn_t ora_lookup_fn;
 uint32_t (*get_plugin_uptime_ms)(void);
} hud_test_usb_context_t;
extern hud_test_usb_context_t context;
bool tud_cdc_connected(void);
uint32_t tud_cdc_write_available(void);
uint32_t tud_cdc_write(const void *, uint32_t);
uint32_t tud_cdc_write_flush(void);
uint32_t tud_cdc_read(void *, uint32_t);
#endif
