#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "1541hud_mailbox.h"
static hud1541_mailbox_t mailbox;
#undef HUD1541_MAILBOX
#define HUD1541_MAILBOX (&mailbox)
#include "../active-plugins/system/1541HUD-Monitor-USB/src/monitor_transport.c"

static bool connected;
static uint8_t storage[512];
bool tud_cdc_connected(void) { return connected; }
uint32_t tud_cdc_write_available(void) { return 64; }
uint32_t tud_cdc_write(const void *p, uint32_t n) { (void)p; return n; }
uint32_t tud_cdc_write_flush(void) { return 0; }
uint32_t tud_cdc_read(void *p, uint32_t n) { (void)p; (void)n; return 0; }
static void *lookup(api_id_t id) { (void)id; return NULL; }
static uint32_t uptime(void) { return 0; }
hud_test_usb_context_t context = { lookup, uptime };

static void reset(void) {
 memset(&mailbox, 0, sizeof(mailbox));
 mailbox.magic = HUD1541_MAILBOX_MAGIC;
 mailbox.version = HUD1541_MAILBOX_VERSION;
 mailbox.led_request_mode = HUD1541_LED_POLICY_AUTO;
 connected = true;
 monitor_transport_init();
 tx.data = storage; tx.capacity = sizeof(storage);
 hud1541_led_ack_seq = 0;
 hud1541_cdc_was_connected = false;
 hud1541_state_pending = false;
 hud1541_next_status_ms = 0;
}

static const char *receive(void) {
 static char text[1024];
 uint32_t total = 0;
 while (hud_tx_used(&tx)) {
  const uint8_t *data;
  uint32_t n = hud_tx_peek(&tx, &data);
  assert(total + n < sizeof(text));
  memcpy(text + total, data, n); total += n;
  assert(hud_tx_consume(&tx, n));
 }
 text[total] = 0;
 return text;
}
static const char *command(const char *line) {
 transport_command(line, NULL);
 return receive();
}

int main(void) {
 reset();
 assert(!strcmp(command("HUDLED?"), "$HUDLED,OK,MODE=AUTO,SAVED=1\r\n"));
 mailbox.last_motor = 1;
 assert(strstr(command("HUDLED=OFF"), "$HUDLED,PENDING,MODE=OFF,SAVED=0,WAIT=MOTOR_STOP"));
 assert(mailbox.led_request_seq == 1);
 assert(strstr(command("HUDLED?"), "PENDING,MODE=OFF,SAVED=0,WAIT=MOTOR_STOP"));
 mailbox.last_motor = 0;
 assert(strstr(command("HUDLED?"), "PENDING,MODE=OFF,SAVED=0,WAIT=CAPTURE_IDLE"));
 /* Result visible before the completion sequence is still pending. */
 mailbox.led_result = 1;
 assert(strstr(command("HUDLED?"), "PENDING,MODE=OFF,SAVED=0"));
 mailbox.led_applied_seq = mailbox.led_request_seq;
 assert(!strcmp(command("HUDLED?"), "$HUDLED,OK,MODE=OFF,SAVED=1\r\n"));
 /* A new request must not inherit the old request's successful result/ack. */
 assert(strstr(command("HUDLED=AUTO"), "PENDING,MODE=AUTO,SAVED=0"));
 assert(mailbox.led_request_seq == 2);
 hud1541_serial_task();
 assert(!strstr(receive(), "$HUDLED,OK"));
 assert(hud1541_led_ack_seq == 1);
 mailbox.led_result = 2;
 mailbox.led_applied_seq = mailbox.led_request_seq;
 assert(!strcmp(command("HUDLED?"), "$HUDLED,FAIL,MODE=AUTO,SAVED=0\r\n"));
 hud1541_serial_task();
 assert(strstr(receive(), "$HUDLED,FAIL,MODE=AUTO,SAVED=0"));
 assert(hud1541_led_ack_seq == 2);
 /* A matching sequence with an incomplete result must not mean saved. */
 mailbox.led_result = 0;
 assert(strstr(command("HUDLED?"), "PENDING,MODE=AUTO,SAVED=0"));
 mailbox.led_request_seq = mailbox.led_applied_seq = 47;
 mailbox.led_result = 1;
 assert(strstr(command("HUDLED=OFF"), "PENDING,MODE=OFF,SAVED=0"));
 assert(mailbox.led_request_seq == 48);
 mailbox.led_request_seq = UINT32_MAX;
 mailbox.led_applied_seq = 1;
 (void)command("HUDLED=AUTO");
 assert(mailbox.led_request_seq == 2);
 mailbox.led_applied_seq = 2;
 mailbox.led_result = 1;
 hud1541_serial_task();
 assert(strstr(receive(), "$HUDLED,OK,MODE=AUTO,SAVED=1"));
 assert(hud1541_led_ack_seq == 2);
 /* Completion acknowledgement must survive queue backpressure and send once. */
 mailbox.led_request_seq = mailbox.led_applied_seq = 3;
 mailbox.led_result = 1;
 char full[511]; memset(full, 'x', sizeof(full));
 assert(hud_tx_enqueue(&tx, full, sizeof(full)));
 hud1541_serial_task();
 assert(hud1541_led_ack_seq == 2);
 (void)receive();
 hud1541_serial_task();
 assert(strstr(receive(), "$HUDLED,OK,MODE=AUTO,SAVED=1"));
 assert(hud1541_led_ack_seq == 3);
 hud1541_serial_task();
 assert(!strstr(receive(), "$HUDLED,"));
 connected = false; monitor_transport_task();
 connected = true; hud1541_serial_task();
 assert(!strstr(receive(), "$HUDLED,FAIL"));
 assert(strstr(command("HUDLED?"), "OK,MODE=AUTO,SAVED=1"));
 puts("Production LED transport: boot, pending, failure, supersession, wrap and ack backpressure passed");
}
