// Copyright (C) 2026 Piers Finlayson <piers@piers.rocks>
//
// MIT License

// One ROM system plugin implementing USB

#include "include.h"
#include "usb_plugin.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "usb_picobootx.h"
#include "drivehud_mailbox.h"

// Optimisations:
// - Move timer handler to library and see if it can be better optimised
// - Add IRQ prioritisation a la SDK

// Define this plugin's attribues
void usb_main(
 ora_lookup_fn_t ora_lookup_fn,
 ora_plugin_type_t plugin_type,
 const ora_entry_args_t *entry_args
);
ORA_SECTION(".plugin_header")
const ora_plugin_header_t ora_plugin_header = {
 .magic = ORA_PLUGIN_MAGIC,
 .api_version = ORA_PLUGIN_VERSION_1,
 .major_version = MAJOR_VERSION,
 .minor_version = MINOR_VERSION,
 .patch_version = PATCH_VERSION,
 .build_version = BUILD_VERSION,
 .entry = usb_main,
 .plugin_type = ORA_PLUGIN_TYPE_SYSTEM,
 .sam_usage = 255,
 .overrides1 = ORA_OVERRIDE1_DISABLE_VBUS_DETECT,
 .properties1 = ORA_PROPERTY1_SUPPORTS_USB_RUNNING | ORA_PROPERTY1_SUPPORTS_YIELD,
 .min_fw_major_version = 0,
 .min_fw_minor_version = 7,
 .min_fw_patch_version = 0,
 .reserved = {0},
};

// Plugin context, stored in .bss
usb_plugin_context_t context;

// DriveHUD V0.0.17 CDC event bridge; implementation is below.
static void drivehud_serial_task(void);

void init_data_bss(void) {
 extern uint32_t __ramfunc_start;
 extern uint32_t __ramfunc_end;
 extern uint32_t __ramfunc_load;
 extern uint32_t __data_start;
 extern uint32_t __data_end;
 extern uint32_t __data_load;
 extern uint32_t __bss_start;
 extern uint32_t __bss_end;

 // Copy .ramfunc from LMA (flash) to VMA (RAM)
 uint32_t *src = &__ramfunc_load;
 uint32_t *dst = &__ramfunc_start;
 while (dst < &__ramfunc_end) {
 *dst++ = *src++;
 }

 // Copy .data from LMA (flash) to VMA (RAM)
 src = &__data_load;
 dst = &__data_start;
 while (dst < &__data_end) {
 *dst++ = *src++;
 }

 // Zero .bss
 dst = &__bss_start;
 while (dst < &__bss_end) {
 *dst++ = 0;
 }
}

// Timer0 IRQ handler to increment the timer_ms field in our plugin context
void timer0_irq_0_handler(void) {
 TIMER0_INTR = (1 << 0);
 TIMER0_ALARM0 = TIMER0_TIMELR + 1000;
 context.timer_ms++;
}

// Implement a function to get the current time in milliseconds, which the
// USB stack can use for timing.
uint32_t board_millis(void) {
 return context.timer_ms;
}

// tinyusb's name for it
uint32_t tusb_time_millis_api(void) {
 return board_millis();
}

void setup_timer0(uint32_t clkref_mhz) {
 // Release TIMER0 from reset
 RESET_RESET &= ~RESET_TIMER0;
 while (!(RESET_DONE & RESET_TIMER0));

 // Set up TICKS
 TICKS_TIMER0_CYCLES = clkref_mhz;
 TICKS_TIMER0_CTRL = 1; 

 // Enable alarm 0 interrupt
 // ORA_IRQ_TIMER0_IRQ_0 corresponds to bit 0 in TIMER0_INTE
 TIMER0_INTE |= (1 << (ORA_IRQ_TIMER0_IRQ_0 % 4));

 // Fire first alarm 1ms from now
 TIMER0_ALARM0 = TIMER0_TIMELR + 1000;
}

void usb_plugin_task(void) {
 // Handle incoming pending command
 if (context.pending.cmd != ONEROM_PENDING_NONE) {
 switch (context.pending.cmd) {
 case ONEROM_PENDING_SET_LED:
 led_handle_pending_set();
 break;

 default:
 LOG("usb_plugin_task: unhandled pending cmd %u", context.pending.cmd);
 break;
 }
 context.pending.cmd = ONEROM_PENDING_NONE;
 }

 led_handle_ongoing_led_modes();

 // ONEROM_CMD_GPIO_SET is applied in the dispatch handler; only the timed
 // release of a bounded hold is deferred to here, where the millisecond
 // timer can be checked.
 gpio_handle_pending_releases();

 drivehud_serial_task();
}



// ---------------------------------------------------------------------------
// DriveHUD V0.0.30 CDC event bridge
// ---------------------------------------------------------------------------
static uint32_t drivehud_next_status_ms;
static bool drivehud_cdc_was_connected;
static bool drivehud_state_pending;
static char *drivehud_append_str(char *p,char *e,const char*s){while(*s&&p<e)*p++=*s++;return p;}
static char *drivehud_append_u32(char*p,char*e,uint32_t v){char t[10];uint32_t n=0;do{t[n++]=(char)('0'+v%10u);v/=10u;}while(v&&n<10);while(n&&p<e)*p++=t[--n];return p;}
static char *drivehud_append_hex8(char*p,char*e,uint8_t v){static const char h[]="0123456789ABCDEF";if(p<e)*p++=h[v>>4];if(p<e)*p++=h[v&15];return p;}

static uint32_t drivehud_track_last(uint32_t s){return s&0xFFu;}
static uint32_t drivehud_track_write_valid(uint32_t s){return (s>>8)&1u;}
static uint32_t drivehud_track_pos2(uint32_t s){return (s>>16)&0xFFu;}
static uint32_t drivehud_track_pos_valid(uint32_t s){return (s>>24)&1u;}

/* Never consume an event until the complete text record fits in TinyUSB's
 * current CDC FIFO. This is the opposite of the old sampler printf problem:
 * capture has already happened, and back-pressure only delays telemetry. */
static bool drivehud_cdc_try_send(const char*b,uint32_t l){
 if(!tud_cdc_connected())return false;
 if(tud_cdc_write_available()<l)return false;
 if(tud_cdc_write(b,l)!=l)return false;
 tud_cdc_write_flush();
 return true;
}

static void drivehud_serial_task(void){
 volatile drivehud_mailbox_t*m=DRIVEHUD_MAILBOX;
 char line[160],*p,*e=line+sizeof(line);
 bool cdc_now=tud_cdc_connected();
 bool just_connected=false;

 if(!cdc_now){
 drivehud_cdc_was_connected=false;
 return;
 }
 if(!drivehud_cdc_was_connected){
 drivehud_cdc_was_connected=true;
 just_connected=true;
 drivehud_state_pending=true;
 /* Historical queue entries are not current state. Drop any backlog that
  * accumulated while no GUI/terminal was connected. The RP2350 SRAM snapshot
  * below is authoritative for reconnect initialization.
  */
 m->event_tail=m->event_head;
 drivehud_next_status_ms=0u;
 }

 if(m->magic!=DRIVEHUD_MAILBOX_MAGIC||m->version!=DRIVEHUD_MAILBOX_VERSION){
 static const char w[]="STATUS V0.0.30 WAITING\r\n";
 uint32_t now=board_millis();
 if((int32_t)(now-drivehud_next_status_ms)>=0){
 if(drivehud_cdc_try_send(w,sizeof(w)-1u))drivehud_next_status_ms=now+1000u;
 }
 return;
 }

 /* STATE is deliberately short and authoritative. It is sent before any live
  * event stream after a CDC reconnect so the GUI can populate immediately.
  */
 if(drivehud_state_pending){
 uint32_t ts=m->track_state;
 p=line;
 p=drivehud_append_str(p,e,"STATE V0.0.30 WPV=");p=drivehud_append_u32(p,e,m->write_protect_valid);
 p=drivehud_append_str(p,e," WP=");p=drivehud_append_u32(p,e,m->last_write_protect);
 p=drivehud_append_str(p,e," TPV=");p=drivehud_append_u32(p,e,drivehud_track_pos_valid(ts));
 p=drivehud_append_str(p,e," TP2=");p=drivehud_append_u32(p,e,drivehud_track_pos2(ts));
 p=drivehud_append_str(p,e," TV=");p=drivehud_append_u32(p,e,drivehud_track_write_valid(ts));
 p=drivehud_append_str(p,e," T=");p=drivehud_append_u32(p,e,drivehud_track_last(ts));
 p=drivehud_append_str(p,e," DV=");p=drivehud_append_u32(p,e,m->density_valid);
 p=drivehud_append_str(p,e," D=");p=drivehud_append_u32(p,e,m->last_density);
 p=drivehud_append_str(p,e," M=");p=drivehud_append_u32(p,e,m->last_motor);
 p=drivehud_append_str(p,e,"\r\n");
 if(!drivehud_cdc_try_send(line,(uint32_t)(p-line)))return;
 drivehud_state_pending=false;
 }

 /* Drain queued mechanism/DOS events first. */
 while(m->event_tail!=m->event_head){
 uint32_t tail=m->event_tail;
 uint32_t w=m->events[tail];
 uint32_t type=(w>>28)&0xFu;
 p=line;

 if(type==DRIVEHUD_EVENT_MOTOR){
 p=drivehud_append_str(p,e,"MOTOR state=");
 p=drivehud_append_u32(p,e,w&1u);
 p=drivehud_append_str(p,e,"\r\n");
 }else if(type==DRIVEHUD_EVENT_PHASE){
 uint32_t oldp=(w>>0)&3u,newp=(w>>2)&3u,delta=(w>>4)&3u,motor=(w>>6)&1u;
 p=drivehud_append_str(p,e,"PHASE old=");p=drivehud_append_u32(p,e,oldp);
 p=drivehud_append_str(p,e," new=");p=drivehud_append_u32(p,e,newp);
 p=drivehud_append_str(p,e," delta=");p=drivehud_append_u32(p,e,delta);
 p=drivehud_append_str(p,e," motor=");p=drivehud_append_u32(p,e,motor);
 p=drivehud_append_str(p,e,"\r\n");
 }else if(type==DRIVEHUD_EVENT_TRACK_WRITE){
 uint8_t data=(uint8_t)(w&0xFFu);
 p=drivehud_append_str(p,e,"TRACK_WRITE addr=$0022 data=$");p=drivehud_append_hex8(p,e,data);
 p=drivehud_append_str(p,e," (");p=drivehud_append_u32(p,e,data);p=drivehud_append_str(p,e,")\r\n");
 }else if(type==DRIVEHUD_EVENT_WRITE_PROTECT){
 p=drivehud_append_str(p,e,"WRITE_PROTECT state=");
 p=drivehud_append_u32(p,e,w&1u);
 p=drivehud_append_str(p,e,"\r\n");
 }else if(type==DRIVEHUD_EVENT_DENSITY){
 p=drivehud_append_str(p,e,"DENSITY state=");
 p=drivehud_append_u32(p,e,w&3u);
 p=drivehud_append_str(p,e,"\r\n");
 }else{
 /* Unknown queue entry: discard it rather than wedging the bridge. */
 m->event_tail=(tail+1u)%DRIVEHUD_EVENT_WORDS;
 continue;
 }

 if(!drivehud_cdc_try_send(line,(uint32_t)(p-line)))break;
 __asm volatile("dmb sy" ::: "memory");
 m->event_tail=(tail+1u)%DRIVEHUD_EVENT_WORDS;
 }

 /* Compact health record. Current mechanism state is carried by STATE,
  * avoiding oversized records that can lose their CR/LF terminator.
  */
 {
 uint32_t now=board_millis();
 if(just_connected || (int32_t)(now-drivehud_next_status_ms)>=0){
 p=line;
 p=drivehud_append_str(p,e,"STATUS V0.0.30 CAP=");p=drivehud_append_u32(p,e,m->capture_count);
 p=drivehud_append_str(p,e," PROD=");p=drivehud_append_u32(p,e,m->produced_total);
 p=drivehud_append_str(p,e," CONS=");p=drivehud_append_u32(p,e,m->consumer_total);
 p=drivehud_append_str(p,e," ROV=");p=drivehud_append_u32(p,e,m->ring_overrun);
 p=drivehud_append_str(p,e," QOV=");p=drivehud_append_u32(p,e,m->event_overflow);
 p=drivehud_append_str(p,e,"\r\n");
 if(drivehud_cdc_try_send(line,(uint32_t)(p-line)))drivehud_next_status_ms=now+1000u;
 }
 }

}

// Resolve the USB serial override from device metadata and widen it into the
// UTF-16 descriptor buffer. Returns the number of code units written, or 0
// when there is no override to apply - either the running firmware predates the
// metadata getter, or no override is set - so the caller falls back to the
// chip-ID serial.
//
// The override string lives in flash and is read on demand (zero-copy); nothing
// is cached, and the metadata getter is only looked up here, at descriptor
// time. An override longer than max_chars is truncated, so the descriptor
// never overruns. min_fw is unaffected: absence of the getter is handled, not
// required.
size_t usb_get_serial(uint16_t *desc_str, size_t max_chars) {
 ora_get_metadata_str_fn_t get_metadata_str =
 context.ora_lookup_fn(ORA_ID_GET_METADATA_STR);
 if (get_metadata_str == NULL) {
 return 0;
 }

 const char *serial = NULL;
 if (get_metadata_str(ORA_METADATA_KEY_SERIAL_OVERRIDE, &serial) != ORA_RESULT_OK
 || serial == NULL) {
 return 0;
 }

 size_t len = 0;
 while (serial[len] != '\0' && len < max_chars) {
 desc_str[len] = (uint16_t)(uint8_t)serial[len];
 len++;
 }
 return len;
}

void usb_init(ora_lookup_fn_t ora_lookup_fn) {
 // Look up the required functions from the API.
 context.ora_lookup_fn = ora_lookup_fn;
 context.log = ora_lookup_fn(ORA_ID_LOG);
 context.debug = ora_lookup_fn(ORA_ID_DEBUG_LOG);
 context.err_log = ora_lookup_fn(ORA_ID_ERR_LOG);
 ora_register_irq_fn_t register_irq = ora_lookup_fn(ORA_ID_REGISTER_IRQ);
 ora_setup_usb_fn_t setup_usb = ora_lookup_fn(ORA_ID_SETUP_USB);
 ora_enable_irq_fn_t enable_irq = ora_lookup_fn(ORA_ID_ENABLE_IRQ);
 ora_get_clkref_mhz_fn_t get_clkref_mhz = ora_lookup_fn(ORA_ID_GET_CLKREF_MHZ);
 context.set_status_led = ora_lookup_fn(ORA_ID_SET_STATUS_LED);
 context.get_active_ram_slot = ora_lookup_fn(ORA_ID_GET_ACTIVE_RAM_SLOT);
 context.get_ram_slot_info = ora_lookup_fn(ORA_ID_GET_RAM_SLOT_INFO);
 context.read_ram_rom_slot = ora_lookup_fn(ORA_ID_READ_RAM_ROM_SLOT);
 context.reprogram_ram_rom_slot = ora_lookup_fn(ORA_ID_REPROGRAM_RAM_ROM_SLOT);
 // Can't log until we have the log functions
 DEBUG("USB plugin started");

 // Resolve the GPIO API and decide what the GPIO commands can offer. Done
 // once, here, because none of it can change while the plugin runs - and
 // because probing it per request would put ORA lookups on the command path.
 gpio_init_caps();

 // Set up USB. tinyusb will register its own IRQ handler, using the API
 // functions we provide.
 setup_usb();

 // Set up timer0
 register_irq(ORA_IRQ_TIMER0_IRQ_0, timer0_irq_0_handler);
 uint32_t clkref_mhz = get_clkref_mhz();
 setup_timer0(clkref_mhz);
 enable_irq(ORA_IRQ_TIMER0_IRQ_0, 1);

 usb_picoboot_init(EPNUM_VENDOR_OUT, EPNUM_VENDOR_IN);

 tusb_rhport_init_t dev_init = {
 .role = TUSB_ROLE_DEVICE,
 .speed = TUSB_SPEED_AUTO
 };
 tusb_init(BOARD_TUD_RHPORT, &dev_init);

 DEBUG("USB plugin setup complete");
}

// Main plugin entry point
void usb_main(
 ora_lookup_fn_t ora_lookup_fn,
 ora_plugin_type_t plugin_type,
 const ora_entry_args_t *entry_args
) {
 // Unused variables
 (void)plugin_type;
 (void)entry_args;

 // Initialize .ram_func, .data and .bss. Do up-front to avoid
 // accidentally using it first
 init_data_bss();

 // Initialize USB and related functionality
 usb_init(ora_lookup_fn);
 ora_yield_fn_t yield = ora_lookup_fn(ORA_ID_YIELD);

 while (1) {
 tud_task();
 usb_picoboot_task();
 usb_plugin_task();
 yield(NULL);
 }

 ERR("USB plugin exiting");
 return;
}

// Invoked when device is mounted
void tud_mount_cb(void) {
 LOG("USB mounted");
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
 LOG("USB unmounted");
}

void tud_suspend_cb(bool remote_wakeup_en) {
 LOG("USB bus suspended, remote wakeup %s", remote_wakeup_en ? "enabled" : "disabled");
}

void tud_resume_cb(void) {
 LOG("USB bus resumed");
}

// Invoked when CDC data is received
void tud_cdc_rx_cb(uint8_t itf) {
 uint8_t buf[64];
 uint32_t count = tud_cdc_n_read(itf, buf, sizeof(buf));

 LOG("CDC received %u bytes on interface %u", count, itf);
}

// Invoked when a control transfer is received on vendor interface
// Used to respond to MS OS 2.0 descriptor request from Windows
bool tud_vendor_control_xfer_cb(
 uint8_t rhport,
 uint8_t stage,
 tusb_control_request_t const *request
) {
 // Try PICOBOOT first
 if (app_picoboot_control_xfer_cb(rhport, stage, request)) {
 return true;
 }

 // Handle MS OS 2.0 descriptor request, for WCID on Windoows 8.1+. Avoids
 // the need for Zadig to setup WinUSB on Windows.
 if ((request->bRequest == VENDOR_REQUEST_MICROSOFT) &&
 (request->bmRequestType_bit.type == TUSB_REQ_TYPE_VENDOR)) {
 if (stage == CONTROL_STAGE_SETUP) {
 if (request->wIndex == 7) {
 // Return MS OS 2.0 descriptor
 return tud_control_xfer(rhport, request, (void *)desc_ms_os_20, MS_OS_20_DESC_LEN);
 }

 // Unsupported wIndex
 return false;
 } else {
 // Return true for ACK and DATA stages.
 return true;
 }
 }

 return false;
}

#include <sys/stat.h>

void _exit(int status) { (void)status; while(1); }
int _close(int fd) { (void)fd; return -1; }
int _fstat(int fd, struct stat *st) { (void)fd; (void)st; return -1; }
int _isatty(int fd) { (void)fd; return 1; }
int _lseek(int fd, int offset, int whence) { (void)fd; (void)offset; (void)whence; return -1; }
int _read(int fd, char *buf, int len) { (void)fd; (void)buf; (void)len; return -1; }
int _write(int fd, char *buf, int len) { (void)fd; (void)buf; (void)len; return -1; }
int _sbrk(int incr) { (void)incr; return -1; }
int _kill(int pid, int sig) { (void)pid; (void)sig; return -1; }
int _getpid(void) { return 1; }

#include <stdint.h>
#include <stdarg.h>

// panic - called by dcd_rp2040 on unrecoverable error
void panic(const char *fmt, ...) {
 (void)fmt;
 while (1);
}

// IRQ functions - dcd_rp2040 calls these but the plugin framework
// owns IRQ registration. The USB IRQ is already registered before
// tusb_init is called, so these can be no-ops.
void irq_add_shared_handler(uint32_t num, ora_irq_handler_t handler, uint8_t order) {
 (void)order;
 ora_register_irq_fn_t register_irq = context.ora_lookup_fn(ORA_ID_REGISTER_IRQ);

 // Pico SDK declares handlers as void* but we store them as function
 // pointers. This cast is safe because tinyusb always passes genuine
 // function pointers here.
 register_irq(num, handler);
}

void irq_remove_handler(uint32_t num, ora_irq_handler_t handler) {
 (void)handler;
 ora_register_irq_fn_t register_irq = context.ora_lookup_fn(ORA_ID_REGISTER_IRQ);
 register_irq(num, NULL);
}

void irq_set_enabled(uint32_t num, bool enabled) {
 ora_enable_irq_fn_t enable_irq = context.ora_lookup_fn(ORA_ID_ENABLE_IRQ);
 enable_irq(num, enabled ? 1 : 0);
}

void __assert_func(const char *file, int line, const char *func, const char *expr) {
 ERR("Assertion failed: %s, at %s:%d in function %s", expr, file, line, func);
 while (1);
}
