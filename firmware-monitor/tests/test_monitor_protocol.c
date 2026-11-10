#include "monitor_protocol.h"
#include "monitor_queue.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static hud_command_t commands[8];
static unsigned count;
static void receive(const char *line, void *unused) {
    (void)unused;
    assert(count < 8);
    commands[count++] = hud_parse_command(line);
}
int main(void) {
    hud_rx_t rx = {0};
    const char *fragmented = "HUDLED=OFF\r\nHUDCFG M=31\nHUDLED?\n";
    for (size_t i = 0; i < strlen(fragmented); i++)
        hud_rx_feed(&rx, (const uint8_t *)fragmented+i, 1, receive, NULL);
    assert(count == 3 && commands[0].kind == HUD_CMD_LED_SET);
    assert(commands[0].value == 0x5a && commands[1].kind == HUD_CMD_CONFIG);
    assert(commands[1].value == 31 && commands[2].kind == HUD_CMD_LED_QUERY);
    assert(hud_parse_command("HUDCFG M=32").kind == HUD_CMD_INVALID);
    assert(hud_parse_command("HUDCFG M=31x").kind == HUD_CMD_INVALID);
    assert(hud_parse_command("HUDCFG M=").kind == HUD_CMD_INVALID);
    assert(hud_parse_command("xHUDLED=AUTO").kind == HUD_CMD_INVALID);
    assert(hud_parse_command("HUDLED=AUTO").value == 0xa5);
    assert(hud_parse_command("HUDCFG M=0").value == 0);
    assert(hud_parse_command("HUDINFO?").kind == HUD_CMD_INFO);
    assert(hud_parse_command("HUDDIAG?").kind == HUD_CMD_DIAG);
    // Reject an oversized line in its entirety, then recover at newline.
    char oversized[100]; memset(oversized, 'x', sizeof(oversized));
    hud_rx_feed(&rx, (uint8_t *)oversized, sizeof(oversized), receive, NULL);
    assert(rx.errors == 1 && count == 3);
    const char *recovery="\nHUDLED?\nHUDINFO?\n";
    hud_rx_feed(&rx,(const uint8_t *)recovery,strlen(recovery),receive,NULL);
    assert(count == 5 && commands[3].kind == HUD_CMD_LED_QUERY);
    assert(commands[4].kind == HUD_CMD_INFO);
    const uint8_t nul_line[]={'H',0,'U','D','\n'};
    hud_rx_feed(&rx,nul_line,sizeof(nul_line),receive,NULL);
    assert(count == 5 && rx.errors == 2);
    uint8_t buffer[8]; hud_tx_t tx={.data=buffer,.capacity=8};
    assert(hud_tx_enqueue(&tx,"abcdef",6));
    assert(!hud_tx_enqueue(&tx,"xx",2) && tx.waits == 1);
    assert(!hud_tx_enqueue(&tx,"12345678",8) && tx.oversize == 1);
    const uint8_t *part;
    assert(hud_tx_peek(&tx,&part)==6 && !memcmp(part,"abcdef",6));
    assert(hud_tx_consume(&tx,4));
    assert(hud_tx_enqueue(&tx,"ghijk",5));
    assert(hud_tx_used(&tx)==7);
    assert(!hud_tx_consume(&tx,8));
    assert(hud_tx_peek(&tx,&part)==4 && !memcmp(part,"efgh",4));
    assert(hud_tx_consume(&tx,4));
    assert(hud_tx_peek(&tx,&part)==3 && !memcmp(part,"ijk",3));
    assert(hud_tx_consume(&tx,3) && hud_tx_used(&tx)==0);
    puts("Monitor tests: framing, command bounds/recovery, TX backpressure, partial drains, wrap and bounds passed");
}
