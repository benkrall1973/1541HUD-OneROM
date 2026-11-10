// Exercise the production Host-Control wait implementation on a native host.
// Build with ORA_HOST_TEST, function/data sections, and linker GC.
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../firmware-control/active-plugins/user/1541HUD-Host-Control-v0.8.1/src/host_control_main.c"

static volatile uint32_t capture[64];
static volatile uint32_t *write_cursor;
static unsigned yields;
static bool feed_when_idle;

void ora_host_test_yield(void) {}

static ora_result_t fake_yield(uint8_t *paused) {
    yields++;
    if (paused) *paused = 1;
    if (feed_when_idle) {
        capture[0] = 0x11; capture[1] = 0x22;
        capture[2] = 0x33; capture[3] = 0x44;
        write_cursor = capture + 4;
        feed_when_idle = false;
    }
    return ORA_RESULT_OK;
}

static void *lookup(api_id_t id) {
    return id == ORA_ID_YIELD ? (void *)fake_yield : NULL;
}

int main(void) {
    ORA_KNOCK_DECLARE(knock, 2);
    knock->mask = 0xff; knock->len = 2; knock->data_size = 32;
    knock->x_mask = 0x100; knock->matches[0] = 0x11; knock->matches[1] = 0x22;
    s_lookup = lookup; s_write_pos_ptr = &write_cursor;
    uint32_t payload[2];
    volatile uint32_t *next;

    // Empty capture ring must cooperate, then consume a complete command.
    write_cursor = capture; feed_when_idle = true; yields = 0;
    assert(wait_for_knock_cooperative(knock, capture, 6, 1, payload, 2, NULL, &next) == ORA_RESULT_OK);
    assert(yields == 1 && payload[0] == 0x33 && payload[1] == 0x44 && next == capture + 4);

    // Ready data must not yield; verify wrap, overlap restart, and X filtering.
    capture[61] = 0x11; capture[62] = 0x11; capture[63] = 0x122;
    capture[0] = 0x22; capture[1] = 0x55; capture[2] = 0x66;
    write_cursor = capture + 3; yields = 0;
    assert(wait_for_knock_cooperative(knock, capture, 6, 1, payload, 2, capture + 61, &next) == ORA_RESULT_OK);
    assert(yields == 0 && payload[0] == 0x55 && payload[1] == 0x66 && next == capture + 3);

    assert(wait_for_knock_cooperative(NULL, capture, 6, 1, payload, 2, NULL, &next) == ORA_RESULT_INVALID_ARG);
    puts("cooperative knock: idle yield, complete payload, wrap, overlap, debounce and invalid argument checks passed");
    return 0;
}
