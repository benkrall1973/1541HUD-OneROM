// 1541HUD telemetry transport layered on official OneROM v0.8.1 USB.
// The shared mailbox and existing V51 record formats are preserved.
#include "usb_plugin.h"
#include "1541hud_mailbox.h"
#include "monitor_transport.h"
#include "monitor_protocol.h"
#include "monitor_queue.h"
#include <string.h>
#if defined(ORA_HOST_TEST)
#define HUD_BARRIER() __asm volatile("" ::: "memory")
#else
#define HUD_BARRIER() __asm volatile("dmb sy" ::: "memory")
#endif
#define HUD1541_EVENT_SYNC_DIAG 8u
#define HUD1541_TELEM_SHIFT 8u
#define HUD1541_TELEM_ALL 0x1fu
#define HUD1541_TELEM_FIELD (HUD1541_TELEM_ALL << HUD1541_TELEM_SHIFT)
static uint8_t tx_fallback[128];
static hud_tx_t tx;
static hud_rx_t rx;
static uint32_t hud1541_next_status_ms;
static bool hud1541_cdc_was_connected;
static bool hud1541_state_pending;
static uint32_t hud1541_led_request_seq;
static uint32_t hud1541_led_ack_seq;
#define HUD1541_LED_POLICY_AUTO 0xA5u
#define HUD1541_LED_POLICY_OFF  0x5Au
static char *hud1541_append_str(char *p,char *e,const char*s){while(*s&&p<e)*p++=*s++;return p;}
static char *hud1541_append_u32(char*p,char*e,uint32_t v){char t[10];uint32_t n=0;do{t[n++]=(char)('0'+v%10u);v/=10u;}while(v&&n<10);while(n&&p<e)*p++=t[--n];return p;}
static char *hud1541_append_hex8(char*p,char*e,uint8_t v){static const char h[]="0123456789ABCDEF";if(p<e)*p++=h[v>>4];if(p<e)*p++=h[v&15];return p;}

static uint32_t hud1541_track_last(uint32_t s){return s&0xFFu;}
static uint32_t hud1541_track_write_valid(uint32_t s){return (s>>8)&1u;}
static uint32_t hud1541_track_pos2(uint32_t s){return (s>>16)&0xFFu;}
static uint32_t hud1541_track_pos_valid(uint32_t s){return (s>>24)&1u;}

/* Never consume an event until the complete text record fits in TinyUSB's
 * current CDC FIFO. This is the opposite of the old sampler printf problem:
 * capture has already happened, and back-pressure only delays telemetry. */
static bool hud1541_cdc_try_send(const char *data, uint32_t size) {
 if (!tud_cdc_connected()) return false;
 return hud_tx_enqueue(&tx, data, size);
}

static void hud1541_serial_task(void){
 volatile hud1541_mailbox_t*m=HUD1541_MAILBOX;
 char line[160],*p,*e=line+sizeof(line);
 bool cdc_now=tud_cdc_connected();
 bool just_connected=false;

 if(!cdc_now){
 /* No host is present to consume telemetry.  Keep the producer's bounded
  * queue empty rather than manufacturing overflow warnings while the HUD is
  * closed. The mailbox snapshot remains authoritative at reconnect. */
 m->event_tail=m->event_head;
 hud1541_cdc_was_connected=false;
 return;
 }
 if(!hud1541_cdc_was_connected){
 hud1541_cdc_was_connected=true;
 just_connected=true;
 hud1541_state_pending=true;
 /* Historical queue entries are not current state. Drop any backlog that
  * accumulated while no GUI/terminal was connected. The RP2350 SRAM snapshot
  * below is authoritative for reconnect initialization.
  */
 m->event_tail=m->event_head;
 hud1541_next_status_ms=0u;
 }

 if(m->magic!=HUD1541_MAILBOX_MAGIC||m->version!=HUD1541_MAILBOX_VERSION){
 static const char w[]="STATUS T0.0.16 WAITING\r\n";
 uint32_t now=context.get_plugin_uptime_ms();
 if((int32_t)(now-hud1541_next_status_ms)>=0){
 if(hud1541_cdc_try_send(w,sizeof(w)-1u))hud1541_next_status_ms=now+1000u;
 }
 return;
 }

 if(m->led_applied_seq!=hud1541_led_ack_seq){
 p=line;
 p=hud1541_append_str(p,e,m->led_result==1u?"$HUDLED,OK,MODE=":"$HUDLED,FAIL,MODE=");
 p=hud1541_append_str(p,e,m->led_request_mode==HUD1541_LED_POLICY_OFF?"OFF":"AUTO");
 p=hud1541_append_str(p,e,m->led_result==1u?",SAVED=1\r\n":",SAVED=0\r\n");
 if(!hud1541_cdc_try_send(line,(uint32_t)(p-line)))return;
 hud1541_led_ack_seq=m->led_applied_seq;
 }

 /* STATE is deliberately short and authoritative. It is sent before any live
  * event stream after a CDC reconnect so the GUI can populate immediately.
  */
 if(hud1541_state_pending){
 uint32_t ts=m->track_state;
 p=line;
 p=hud1541_append_str(p,e,"STATE T0.0.16 WPV=");p=hud1541_append_u32(p,e,m->write_protect_valid);
 p=hud1541_append_str(p,e," WP=");p=hud1541_append_u32(p,e,m->last_write_protect);
 p=hud1541_append_str(p,e," TPV=");p=hud1541_append_u32(p,e,hud1541_track_pos_valid(ts));
 p=hud1541_append_str(p,e," TP2=");p=hud1541_append_u32(p,e,hud1541_track_pos2(ts));
 p=hud1541_append_str(p,e," TV=");p=hud1541_append_u32(p,e,hud1541_track_write_valid(ts));
 p=hud1541_append_str(p,e," T=");p=hud1541_append_u32(p,e,hud1541_track_last(ts));
 p=hud1541_append_str(p,e," DV=");p=hud1541_append_u32(p,e,m->density_valid);
 p=hud1541_append_str(p,e," D=");p=hud1541_append_u32(p,e,m->last_density);
 p=hud1541_append_str(p,e," M=");p=hud1541_append_u32(p,e,m->last_motor);
 p=hud1541_append_str(p,e,"\r\n");
 if(!hud1541_cdc_try_send(line,(uint32_t)(p-line)))return;
 hud1541_state_pending=false;
 }

 /* Health is sent before the unbounded live-event drain. During sustained
  * reads, headers can continuously refill that queue; putting STATUS after
  * it starves the HUD-health display despite a healthy capture path.
  *
  * Keep this record below TinyUSB's 64-byte full-speed TX FIFO.  The send
  * helper intentionally refuses partial records, and the former verbose
  * CAP/PROD/CONS/ROV/QOV form could exceed that FIFO once counters grew.
  * C/R/Q are the three values the desktop HUD actually displays. */
 {
 uint32_t now=context.get_plugin_uptime_ms();
 if(just_connected || (int32_t)(now-hud1541_next_status_ms)>=0){
 p=line;
 p=hud1541_append_str(p,e,"STATUS T0.0.16 C=");p=hud1541_append_u32(p,e,m->capture_count);
 p=hud1541_append_str(p,e," R=");p=hud1541_append_u32(p,e,m->ring_overrun);
 p=hud1541_append_str(p,e," Q=");p=hud1541_append_u32(p,e,m->event_overflow);
 p=hud1541_append_str(p,e,"\r\n");
 if(hud1541_cdc_try_send(line,(uint32_t)(p-line)))hud1541_next_status_ms=now+1000u;

 /* Keep low-level capture diagnostics in a separate short record so the
  * normal STATUS record remains compatible with 1541HUD and each CDC write
  * stays below TinyUSB's 64-byte FIFO. */
 p=line;
 p=hud1541_append_str(p,e,"DIAG P=");p=hud1541_append_u32(p,e,m->pio_pc);
 p=hud1541_append_str(p,e," X=");p=hud1541_append_u32(p,e,m->rx_level);
 p=hud1541_append_str(p,e," B=");p=hud1541_append_u32(p,e,m->dma_busy);
 p=hud1541_append_str(p,e," N=");p=hud1541_append_u32(p,e,m->dma_trans_count);
 p=hud1541_append_str(p,e,"\r\n");
 (void)hud1541_cdc_try_send(line,(uint32_t)(p-line));
 }
 }

 /* Drain queued mechanism/DOS events. */
 while(m->event_tail!=m->event_head){
 uint32_t tail=m->event_tail;
 uint32_t w=m->events[tail];
 uint32_t type=(w>>28)&0xFu;
 p=line;

 if(type==HUD1541_EVENT_MOTOR){
 p=hud1541_append_str(p,e,"MOTOR state=");
 p=hud1541_append_u32(p,e,w&1u);
 p=hud1541_append_str(p,e,"\r\n");
 }else if(type==HUD1541_EVENT_PHASE){
 uint32_t oldp=(w>>0)&3u,newp=(w>>2)&3u,delta=(w>>4)&3u,motor=(w>>6)&1u;
 p=hud1541_append_str(p,e,"PHASE old=");p=hud1541_append_u32(p,e,oldp);
 p=hud1541_append_str(p,e," new=");p=hud1541_append_u32(p,e,newp);
 p=hud1541_append_str(p,e," delta=");p=hud1541_append_u32(p,e,delta);
 p=hud1541_append_str(p,e," motor=");p=hud1541_append_u32(p,e,motor);
 p=hud1541_append_str(p,e,"\r\n");
 }else if(type==HUD1541_EVENT_TRACK_WRITE){
 uint8_t data=(uint8_t)(w&0xFFu);
 p=hud1541_append_str(p,e,"TRACK_WRITE addr=$0022 data=$");p=hud1541_append_hex8(p,e,data);
 p=hud1541_append_str(p,e," (");p=hud1541_append_u32(p,e,data);p=hud1541_append_str(p,e,")\r\n");
 }else if(type==HUD1541_EVENT_WRITE_PROTECT){
 p=hud1541_append_str(p,e,"WRITE_PROTECT state=");
 p=hud1541_append_u32(p,e,w&1u);
 p=hud1541_append_str(p,e,"\r\n");
 }else if(type==HUD1541_EVENT_DENSITY){
 p=hud1541_append_str(p,e,"DENSITY state=");
 p=hud1541_append_u32(p,e,w&3u);
 p=hud1541_append_str(p,e,"\r\n");
 }else if(type==HUD1541_EVENT_WRITE_GATE){
 p=hud1541_append_str(p,e,"WRITE_GATE state=");
 p=hud1541_append_u32(p,e,w&1u);
 p=hud1541_append_str(p,e,"\r\n");
 }else if(type==HUD1541_EVENT_HDRPHY_DIAG){
 uint32_t track=(w>>21)&0x7Fu;
 uint32_t sector=(w>>16)&0x1Fu;
 uint32_t timestamp_us_mod=(w&0xFFFFu)<<4;
 p=hud1541_append_str(p,e,"HDRPHY T0.0.16 T=");
 p=hud1541_append_u32(p,e,track);
 p=hud1541_append_str(p,e," S=");
 p=hud1541_append_u32(p,e,sector);
 p=hud1541_append_str(p,e," US=");
 p=hud1541_append_u32(p,e,timestamp_us_mod);
 p=hud1541_append_str(p,e,"\r\n");
}else if(type==HUD1541_EVENT_HDRMETA_DIAG){
 uint8_t valid=(uint8_t)((w>>27)&1u);
 uint8_t checksum=(uint8_t)((w>>16)&0xFFu);
 uint8_t id1=(uint8_t)((w>>8)&0xFFu);
 uint8_t id2=(uint8_t)(w&0xFFu);
 p=hud1541_append_str(p,e,"HDRMETA T0.0.16 ID1=$");
 p=hud1541_append_hex8(p,e,id1);
 p=hud1541_append_str(p,e," ID2=$");
 p=hud1541_append_hex8(p,e,id2);
 p=hud1541_append_str(p,e," CHK=$");
 p=hud1541_append_hex8(p,e,checksum);
 p=hud1541_append_str(p,e," OK=");
 p=hud1541_append_u32(p,e,valid);
 p=hud1541_append_str(p,e,"\r\n");
}else if(type==HUD1541_EVENT_RPM_DIAG){
 uint32_t track=(w>>21)&0x7Fu;
 uint32_t revs=(w>>16)&0x1Fu;
 uint32_t rpm100=w&0xFFFFu;
 p=hud1541_append_str(p,e,"RPM T0.0.17 T=");
 p=hud1541_append_u32(p,e,track);
 p=hud1541_append_str(p,e," REV=");
 p=hud1541_append_u32(p,e,revs);
 p=hud1541_append_str(p,e," RPM=");
 p=hud1541_append_u32(p,e,rpm100/100u);
 p=hud1541_append_str(p,e,".");
 if((rpm100%100u)<10u) p=hud1541_append_str(p,e,"0");
 p=hud1541_append_u32(p,e,rpm100%100u);
 p=hud1541_append_str(p,e,"\r\n");
 }else if(type==HUD1541_EVENT_SYNC_DIAG){
 uint32_t level=(w>>27)&1u;
 uint32_t count=w&0x07FFFFFFu;
 p=hud1541_append_str(p,e,"SYNC T0.0.16 COUNT=");
 p=hud1541_append_u32(p,e,count);
 p=hud1541_append_str(p,e," LEVEL=");
 p=hud1541_append_u32(p,e,level);
 p=hud1541_append_str(p,e,"\r\n");
 }else{
 /* Unknown queue entry: discard it rather than wedging the bridge. */
 m->event_tail=(tail+1u)%HUD1541_EVENT_WORDS;
 continue;
 }

 if(!hud1541_cdc_try_send(line,(uint32_t)(p-line)))break;
 HUD_BARRIER();
 m->event_tail=(tail+1u)%HUD1541_EVENT_WORDS;
 }

}



static void transport_drain(void) {
 const uint8_t *data;
 uint32_t size = hud_tx_peek(&tx, &data);
 uint32_t available = tud_cdc_write_available();
 if (size > available) size = available;
 if (size) {
  uint32_t written = tud_cdc_write(data, size);
  (void)hud_tx_consume(&tx, written);
  tud_cdc_write_flush();
 }
}

static void transport_command(const char *line, void *arg) {
 (void)arg;
 hud_command_t command = hud_parse_command(line);
 volatile hud1541_mailbox_t *m = HUD1541_MAILBOX;
 if (m->magic != HUD1541_MAILBOX_MAGIC || m->version != HUD1541_MAILBOX_VERSION) {
  (void)hud1541_cdc_try_send("$HUDERROR,NOT_READY\r\n", 21u);
  return;
 }
 char reply[112], *p = reply, *end = reply + sizeof(reply);
 if (command.kind == HUD_CMD_LED_QUERY || command.kind == HUD_CMD_LED_SET) {
  if (command.kind == HUD_CMD_LED_SET) {
   m->led_request_mode = command.value;
   HUD_BARRIER();
   m->led_request_seq = ++hud1541_led_request_seq;
   p = hud1541_append_str(p, end, "$HUDLED,PENDING,MODE=");
  } else p = hud1541_append_str(p, end, "$HUDLED,OK,MODE=");
  p = hud1541_append_str(p, end,
      m->led_request_mode == HUD1541_LED_POLICY_OFF ? "OFF" : "AUTO");
  if (command.kind == HUD_CMD_LED_QUERY)
   p = hud1541_append_str(p, end, ",SAVED=1\r\n");
  else p = hud1541_append_str(p, end, m->last_motor ?
      ",SAVED=0,WAIT=MOTOR_STOP\r\n" : ",SAVED=0,WAIT=CAPTURE_IDLE\r\n");
 } else if (command.kind == HUD_CMD_CONFIG) {
  m->flags = (m->flags & ~HUD1541_TELEM_FIELD) |
       (command.value << HUD1541_TELEM_SHIFT);
  p = hud1541_append_str(p, end, "$HUDCFG,OK,M=");
  p = hud1541_append_u32(p, end, command.value);
  p = hud1541_append_str(p, end, "\r\n");
 } else if (command.kind == HUD_CMD_INFO) {
  p = hud1541_append_str(p, end, "$HUDINFO,USB_BUILD=");
  p = hud1541_append_u32(p, end, BUILD_VERSION);
  p = hud1541_append_str(p, end, ",ABI=");
  p = hud1541_append_u32(p, end, HUD1541_MAILBOX_VERSION);
  p = hud1541_append_str(p, end, ",TX=");
  p = hud1541_append_u32(p, end, tx.capacity);
  p = hud1541_append_str(p, end, "\r\n");
 } else if (command.kind == HUD_CMD_DIAG) {
  p = hud1541_append_str(p, end, "$HUDDIAG,RXERR=");
  p = hud1541_append_u32(p, end, rx.errors);
  p = hud1541_append_str(p, end, ",TXWAIT=");
  p = hud1541_append_u32(p, end, tx.waits);
  p = hud1541_append_str(p, end, ",TXBIG=");
  p = hud1541_append_u32(p, end, tx.oversize);
  p = hud1541_append_str(p, end, "\r\n");
 } else p = hud1541_append_str(p, end, "$HUDERROR,COMMAND\r\n");
 (void)hud1541_cdc_try_send(reply, (uint32_t)(p-reply));
}

void monitor_transport_init(void) {
 tx.data = tx_fallback; tx.capacity = sizeof(tx_fallback);
 tx.head = tx.tail = 0; tx.waits = tx.oversize = 0;
 memset(&rx, 0, sizeof(rx));
 ora_alloc_mem_fn_t allocate = context.ora_lookup_fn(ORA_ID_ALLOC_MEM);
 ora_alloc_request_t request = { .size = sizeof(request), .max_access_cycles = 2,
     .accept = ORA_MEM_ACCEPT_SLOWER, .bytes = 512 };
 uint32_t address;
 if (allocate && allocate(&request, &address) == ORA_RESULT_OK) {
  tx.data = (uint8_t *)(uintptr_t)address;
  tx.capacity = 512;
 }
}

void monitor_transport_task(void) {
 if (!tud_cdc_connected()) {
  tx.head = tx.tail = 0; rx.length = 0; rx.discard = false;
  hud1541_serial_task();
  return;
 }
 transport_drain();
 uint8_t input[64];
 uint32_t size = tud_cdc_read(input, sizeof(input));
 if (size) hud_rx_feed(&rx, input, size, transport_command, NULL);
 hud1541_serial_task();
 transport_drain();
}
