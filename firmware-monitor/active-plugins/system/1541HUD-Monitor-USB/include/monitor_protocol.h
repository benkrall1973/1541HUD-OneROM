#ifndef HUD_MONITOR_PROTOCOL_H
#define HUD_MONITOR_PROTOCOL_H
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
typedef enum { HUD_CMD_INVALID, HUD_CMD_LED_QUERY, HUD_CMD_LED_SET,
               HUD_CMD_CONFIG, HUD_CMD_INFO, HUD_CMD_DIAG } hud_command_kind_t;
typedef struct { hud_command_kind_t kind; uint32_t value; } hud_command_t;
typedef struct {
    char line[64];
    uint32_t errors;
    uint8_t length;
    bool discard;
} hud_rx_t;
typedef void (*hud_line_fn_t)(const char *, void *);
void hud_rx_feed(hud_rx_t *, const uint8_t *, size_t, hud_line_fn_t, void *);
hud_command_t hud_parse_command(const char *);
#endif
