#include "monitor_protocol.h"
#include <string.h>

void hud_rx_feed(hud_rx_t *rx, const uint8_t *data, size_t size,
                 hud_line_fn_t emit, void *arg) {
    for (size_t i = 0; i < size; i++) {
        uint8_t c = data[i];
        if (c == '\r' || c == '\n') {
            if (!rx->discard && rx->length) {
                rx->line[rx->length] = 0;
                emit(rx->line, arg);
            }
            rx->length = 0;
            rx->discard = false;
        } else if (!rx->discard) {
            if (c == 0 || rx->length >= sizeof(rx->line) - 1u) {
                rx->errors++;
                rx->discard = true;
                rx->length = 0;
            } else rx->line[rx->length++] = (char)c;
        }
    }
}

hud_command_t hud_parse_command(const char *line) {
    hud_command_t command = { HUD_CMD_INVALID, 0 };
    if (!strcmp(line, "HUDLED?")) command.kind = HUD_CMD_LED_QUERY;
    else if (!strcmp(line, "HUDLED=OFF")) {
        command.kind = HUD_CMD_LED_SET; command.value = 0x5au;
    } else if (!strcmp(line, "HUDLED=AUTO")) {
        command.kind = HUD_CMD_LED_SET; command.value = 0xa5u;
    } else if (!strcmp(line, "HUDINFO?")) command.kind = HUD_CMD_INFO;
    else if (!strcmp(line, "HUDDIAG?")) command.kind = HUD_CMD_DIAG;
    else if (!strncmp(line, "HUDCFG M=", 9)) {
        const char *p = line + 9;
        if (!*p) return command;
        uint32_t value = 0;
        while (*p) {
            if (*p < '0' || *p > '9') return command;
            value = value * 10u + (uint32_t)(*p++ - '0');
            if (value > 31u) return command;
        }
        command.kind = HUD_CMD_CONFIG; command.value = value;
    }
    return command;
}
