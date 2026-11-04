// Copyright (C) 2026 Piers Finlayson <piers@piers.rocks>
//
// MIT License

// One ROM system plugin implementing USB

#include "include.h"
#include "usb_plugin.h"
#include "tusb_config.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "usb_picobootx.h"
#include "picobootx.h"
#include "1541hud_mailbox.h"

#define HUD1541_EVENT_SYNC_DIAG 8u
#define HUD1541_TELEM_SHIFT 8u
#define HUD1541_TELEM_ALL 0x1Fu
#define HUD1541_TELEM_FIELD (HUD1541_TELEM_ALL << HUD1541_TELEM_SHIFT)
static void hud1541_serial_task(void);

// Optimisations:
// - Add IRQ prioritisation a la SDK

// Define this plugin's attribues
void usb_main(
    ora_lookup_fn_t ora_lookup_fn,
    ora_plugin_type_t plugin_type,
    const ora_entry_args_t *entry_args
);
ORA_SECTION(".plugin_header")
const ora_plugin_header_t ora_plugin_header = {
    .magic    = ORA_PLUGIN_MAGIC,
    .api_version  = ORA_PLUGIN_VERSION_1,
    .major_version = MAJOR_VERSION,
    .minor_version = MINOR_VERSION,
    .patch_version = PATCH_VERSION,
    .build_version = BUILD_VERSION,
    .entry  = usb_main,
    .plugin_type = ORA_PLUGIN_TYPE_SYSTEM,
    .sam_usage = 255,
    .overrides1 = ORA_OVERRIDE1_DISABLE_VBUS_DETECT,
    /* CDC is exclusively the Monitor protocol.  Core logs remain available
     * through picoboot/DPRAM rather than corrupting telemetry records. */
    .properties1 = ORA_PROPERTY1_SUPPORTS_USB_RUNNING |
                   ORA_PROPERTY1_SUPPORTS_YIELD |
                   ORA_PROPERTY1_EXCLUSIVE_SERIAL_IN,
    .min_fw_major_version = 0,
    .min_fw_minor_version = 8,
    .min_fw_patch_version = 1,
    .usb_controller_dpram_size = CFG_TUD_RP2040_DPRAM_SIZE,
    .reserved = {0},
};
_Static_assert(CFG_TUD_RP2040_DPRAM_SIZE <= ORA_USB_CONTROLLER_DPRAM_MAX,
    "tinyusb's USB DPRAM is more than a plugin can reserve");

// Plugin context, stored in .bss
usb_plugin_context_t context;

// Place the plugin's initialised data and clear its zeroed data.
//
// The addresses come from the plugin's linker script, so this has nothing to
// act on in a host test build — the plugin's data is the host process's, placed
// by its own toolchain before main runs.  The symbols do not exist there
// either, which is why the whole body is compiled out rather than skipped at
// run time.
void init_data_bss(void) {
#if !defined(ORA_HOST_TEST)
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
#endif // !ORA_HOST_TEST
}

uint32_t board_millis(void) {
    return context.get_plugin_uptime_ms();
}

// tinyusb's name for it
uint32_t tusb_time_millis_api(void) {
    return board_millis();
}

void usb_plugin_task(void) {
    // ONEROM_CMD_SET_LED and ONEROM_CMD_GPIO_SET are both applied in the
    // dispatch handler.  Only the timed release of a bounded GPIO hold is
    // deferred to here - an LED mode that runs on is the firmware engine's to
    // keep going, not this plugin's.
    gpio_release_expired_holds();

    // V1.0.0 baseline: preserve the hardware-proven mailbox/CDC behavior.
    hud1541_serial_task();
}

// ---------------------------------------------------------------------------
// 1541HUD V1.0.0 CDC event bridge for OneROM v0.7.3.
// ---------------------------------------------------------------------------
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
static bool hud1541_cdc_try_send(const char*b,uint32_t l){
 if(!tud_cdc_connected())return false;
 if(tud_cdc_write_available()<l)return false;
 if(tud_cdc_write(b,l)!=l)return false;
 tud_cdc_write_flush();
 return true;
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
 uint32_t now=board_millis();
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
 uint32_t now=board_millis();
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
 __asm volatile("dmb sy" ::: "memory");
 m->event_tail=(tail+1u)%HUD1541_EVENT_WORDS;
 }

}


// Number of UTF-16 code units picoboot_get_serial() must be given room for: 16
// hex digits and the NUL it terminates them with.
#define USB_CHIP_ID_SERIAL_UNITS 17

// Yield the device's effective USB serial.  See usb_plugin.h for the contract.
//
// The metadata getter is looked up per call rather than held, because both
// callers are occasional - a descriptor request and a terminal attaching - and
// a held pointer would cost static RAM the plugin has little of.
size_t usb_get_serial(char *out, size_t out_size) {
    if (out == NULL || out_size == 0) {
        return 0;
    }
    out[0] = '\0';

    const char *override = NULL;
    ora_get_metadata_str_fn_t get_metadata_str =
        context.ora_lookup_fn(ORA_ID_GET_METADATA_STR);
    if (get_metadata_str != NULL) {
        // The override string lives in flash and is read in place.  An unset
        // field reports OK with a NULL pointer, which is the common case rather
        // than a failure.
        if (get_metadata_str(ORA_METADATA_KEY_SERIAL_OVERRIDE, &override)
            != ORA_RESULT_OK) {
            override = NULL;
        }
    }

    // An override set to the empty string is treated as no override, so the
    // chip ID is still used.  A device presenting a zero length serial cannot
    // be told apart from its peers by anything selecting on serial, and the
    // metadata accepts an empty string, so this is reachable rather than
    // theoretical.
    if (override != NULL && override[0] == '\0') {
        override = NULL;
    }

    size_t len = 0;
    if (override != NULL) {
        while (override[len] != '\0' && len < out_size - 1) {
            out[len] = override[len];
            len++;
        }
        out[len] = '\0';
        return len;
    }

    // picobootx produces the chip ID as UTF-16, for the descriptor that first
    // wanted it.  Every code unit is a hex digit, so narrowing to ASCII here
    // loses nothing.
    uint16_t units[USB_CHIP_ID_SERIAL_UNITS];
    size_t count = picoboot_get_serial(units, sizeof(units) / sizeof(units[0]));
    while (len < count && len < out_size - 1) {
        out[len] = (char)units[len];
        len++;
    }
    out[len] = '\0';
    return len;
}

void usb_init(ora_lookup_fn_t ora_lookup_fn) {
    // Look up the required functions from the API.
    context.ora_lookup_fn = ora_lookup_fn;
    context.log = ora_lookup_fn(ORA_ID_LOG);
    context.debug = ora_lookup_fn(ORA_ID_DEBUG_LOG);
    context.err_log = ora_lookup_fn(ORA_ID_ERR_LOG);
    ora_setup_usb_fn_t setup_usb = ora_lookup_fn(ORA_ID_SETUP_USB);
    context.get_plugin_uptime_ms = ora_lookup_fn(ORA_ID_GET_PLUGIN_UPTIME_MS);
    context.set_status_led = ora_lookup_fn(ORA_ID_SET_STATUS_LED);
    context.get_active_ram_slot = ora_lookup_fn(ORA_ID_GET_ACTIVE_RAM_SLOT);
    context.get_ram_slot_info = ora_lookup_fn(ORA_ID_GET_RAM_SLOT_INFO);
    context.read_ram_rom_slot = ora_lookup_fn(ORA_ID_READ_RAM_ROM_SLOT);
    context.reprogram_ram_rom_slot = ora_lookup_fn(ORA_ID_REPROGRAM_RAM_ROM_SLOT);
    // Can't log until we have the log functions
    DEBUG("USB plugin started");

    // Resolve the GPIO API and decide what the GPIO commands can offer.  Done
    // once, here, because none of it can change while the plugin runs - and
    // because probing it per request would put ORA lookups on the command path.
    gpio_init_caps();

    // After gpio_init_caps(), which clears the capability word.
    led_init_caps();

    // Resolved once here, with the rest of the one-time API resolution, since
    // none of it changes while the plugin runs.
    log_drain_init();

    // Set up USB.  tinyusb will register its own IRQ handler, using the API
    // functions we provide.
    setup_usb();

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

    // Initialize .ram_func, .data and .bss.  Do up-front to avoid
    // accidentally using it first
    init_data_bss();

    // Initialize USB and related functionality
    usb_init(ora_lookup_fn);
    ora_yield_fn_t yield = ora_lookup_fn(ORA_ID_YIELD);

    while (1) {
        tud_task();
        usb_picoboot_task();
        usb_plugin_task();
        log_drain_task();
        yield(NULL);

        // Nothing in this loop waits on anything a host test can change, so
        // without a seam the loop would never hand control back and the
        // emulation could never be advanced.  Compiles to nothing on a device.
        ORA_TEST_YIELD();
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
    uint32_t i;

    /* HUDLED? is deliberately read-only. The monitor publishes the policy it
     * restored from NV1 in led_request_mode, so this reply neither pauses PIO
     * capture nor schedules another flash write. */
    for (i = 0u; i + 6u < count; ++i) {
        if (buf[i] == 'H' && buf[i + 1u] == 'U' && buf[i + 2u] == 'D' &&
            buf[i + 3u] == 'L' && buf[i + 4u] == 'E' && buf[i + 5u] == 'D' &&
            buf[i + 6u] == '?') {
            char reply[40];
            char *p = reply;
            char *e = reply + sizeof(reply);
            p = hud1541_append_str(p, e, "$HUDLED,OK,MODE=");
            p = hud1541_append_str(p, e,
                HUD1541_MAILBOX->led_request_mode == HUD1541_LED_POLICY_OFF ? "OFF" : "AUTO");
            p = hud1541_append_str(p, e, ",SAVED=1\r\n");
            (void)hud1541_cdc_try_send(reply, (uint32_t)(p - reply));
            break;
        }
    }

    /* HUDCFG M=<decimal mask> selects optional telemetry classes. It is
     * deliberately tiny and one-way: the desktop may reduce monitoring work,
     * but cannot command the 1541 or drive any bus pin. */
    for (i = 0u; i + 9u < count; ++i) {
        if (buf[i] == 'H' && buf[i + 1u] == 'U' && buf[i + 2u] == 'D' &&
            buf[i + 3u] == 'C' && buf[i + 4u] == 'F' && buf[i + 5u] == 'G' &&
            buf[i + 6u] == ' ' && buf[i + 7u] == 'M' && buf[i + 8u] == '=') {
            uint32_t mask = 0u;
            uint32_t j = i + 9u;
            while (j < count && buf[j] >= '0' && buf[j] <= '9') {
                mask = mask * 10u + (uint32_t)(buf[j] - '0');
                ++j;
            }
            HUD1541_MAILBOX->flags = (HUD1541_MAILBOX->flags & ~HUD1541_TELEM_FIELD) |
                ((mask & HUD1541_TELEM_ALL) << HUD1541_TELEM_SHIFT);
            break;
        }
    }
    for (i = 0u; i + 10u < count; ++i) {
        uint32_t mode = 0u;
        if (buf[i] == 'H' && buf[i + 1u] == 'U' && buf[i + 2u] == 'D' &&
            buf[i + 3u] == 'L' && buf[i + 4u] == 'E' && buf[i + 5u] == 'D' &&
            buf[i + 6u] == '=' && buf[i + 7u] == 'O' && buf[i + 8u] == 'F' &&
            buf[i + 9u] == 'F') mode = HUD1541_LED_POLICY_OFF;
        if (buf[i] == 'H' && buf[i + 1u] == 'U' && buf[i + 2u] == 'D' &&
            buf[i + 3u] == 'L' && buf[i + 4u] == 'E' && buf[i + 5u] == 'D' &&
            buf[i + 6u] == '=' && buf[i + 7u] == 'A' && buf[i + 8u] == 'U' &&
            buf[i + 9u] == 'T' && buf[i + 10u] == 'O') mode = HUD1541_LED_POLICY_AUTO;
        if (mode != 0u) {
            HUD1541_MAILBOX->led_request_mode = mode;
            __asm volatile("dmb sy" ::: "memory");
            HUD1541_MAILBOX->led_request_seq = ++hud1541_led_request_seq;
            /* The Monitor applies the visual policy on its next bounded
             * capture pass, but deliberately defers the NV1 write until the
             * motor is off and the capture ring is drained.  Tell the HUD
             * immediately: silence here made AUTO look like a dead button.
             */
            {
                char reply[80];
                char *p = reply;
                char *e = reply + sizeof(reply);
                p = hud1541_append_str(p, e, "$HUDLED,PENDING,MODE=");
                p = hud1541_append_str(p, e,
                    mode == HUD1541_LED_POLICY_OFF ? "OFF" : "AUTO");
                p = hud1541_append_str(p, e,
                    HUD1541_MAILBOX->last_motor ?
                        ",SAVED=0,WAIT=MOTOR_STOP\r\n" :
                        ",SAVED=0,WAIT=CAPTURE_IDLE\r\n");
                (void)hud1541_cdc_try_send(reply, (uint32_t)(p - reply));
            }
            break;
        }
    }
    DEBUG("CDC received %lu bytes on interface %u", (unsigned long)count, itf);
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

    // Handle MS OS 2.0 descriptor request, for WCID on Windows 8.1+. Avoids
    // the need for Zadig to set up WinUSB on Windows.  The conservative
    // diagnostic USB build intentionally omits this Windows-only path.
#if !defined(ONEROM_USB_PI_CONSERVATIVE)
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
#endif

    return false;
}

// ---------------------------------------------------------------------------
// The device's C library and interrupt glue
//
// None of this belongs to a host test build.  The host has a C library of its
// own, and defining these there would take over the process's — _exit above all,
// which would turn an ordinary exit into a hang.  The IRQ shims are called only
// by tinyusb's RP2040 device controller driver, which a host build does not
// compile, and panic and __assert_func are the bare-metal ends of routines the
// host libraries already provide.
// ---------------------------------------------------------------------------

#if !defined(ORA_HOST_TEST)

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
    // pointers.  This cast is safe because tinyusb always passes genuine
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

#endif // !ORA_HOST_TEST
