#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint32_t hud1541_test_now_us;
#ifndef HUD_MONITOR_SOURCE
#define HUD_MONITOR_SOURCE "../active-plugins/user/1541HUD-Monitor/src/hud1541_monitor_main.c"
#endif
#include HUD_MONITOR_SOURCE

typedef struct {
 hud1541_mailbox_t mailbox;
 uint8_t phase_valid, last_phase, motor_valid, last_motor, wp_valid, last_wp;
 uint8_t density_valid, last_density, gate_valid, last_gate, track_valid, track;
 uint8_t pos_valid, pos2, drvst, stepping;
 uint8_t state, hdr_track, sector, checksum, id1, divider;
 uint16_t ticks;
 uint8_t rpm_valid, rpm_track, rpm_sector, rpm_id1, rpm_id2, revolutions;
 uint32_t start, last;
 uint16_t intervals;
} fixture_t;

static void init(fixture_t *f, uint32_t mask) {
 memset(f, 0, sizeof(*f));
 f->motor_valid = f->last_motor = 1;
 f->mailbox.flags = mask << HUD1541_TELEM_SHIFT;
 hud1541_test_now_us = 123456;
}

static void write_ram(fixture_t *f, uint16_t address, uint8_t data) {
 static const uint8_t address_pins[] = {23,22,21,20,19,18,17,16,15,14,13,11,12};
 static const uint8_t data_pins[] = {7,6,5,0,1,2,3,4};
 uint32_t snapshot = MASK_nCS2;
 for (unsigned i = 0; i < 13; ++i) snapshot |= ((address >> i) & 1u) << address_pins[i];
 for (unsigned i = 0; i < 8; ++i) snapshot |= ((data >> i) & 1u) << data_pins[i];
 assert(logical_addr13_from_gpio(snapshot) == address);
 assert(logical_data_from_gpio(snapshot) == data);
 decode_snapshot(&f->mailbox, snapshot,
  &f->phase_valid, &f->last_phase, &f->motor_valid, &f->last_motor,
  &f->wp_valid, &f->last_wp, &f->density_valid, &f->last_density,
  &f->gate_valid, &f->last_gate, &f->track_valid, &f->track,
  &f->pos_valid, &f->pos2, &f->drvst, &f->stepping,
  &f->state, &f->hdr_track, &f->sector, &f->checksum, &f->id1, &f->divider,
  &f->ticks, &f->rpm_valid, &f->rpm_track, &f->rpm_sector, &f->rpm_id1,
  &f->rpm_id2, &f->revolutions, &f->start, &f->last, &f->intervals);
}

static void header(fixture_t *f, uint8_t checksum) {
 write_ram(f, RAM_HDRTRK, 18);
 write_ram(f, RAM_HDRSEC, 17);
 write_ram(f, RAM_HDRCHK, checksum);
 write_ram(f, RAM_HDRID1, 0xA5);
 write_ram(f, RAM_HDRID2, 0x5A);
}

static unsigned count(const fixture_t *f, uint32_t type) {
 unsigned n = 0;
 for (uint32_t i = f->mailbox.event_tail; i != f->mailbox.event_head;
      i = (i + 1) % HUD1541_EVENT_WORDS)
  if ((f->mailbox.events[i] >> 28) == type) ++n;
 return n;
}

int main(void) {
 fixture_t f;
 init(&f, HUD1541_TELEM_ALL);
 header(&f, 0xFC); /* 18 XOR 17 XOR A5 XOR 5A */
 assert(count(&f, HUD1541_EVENT_HDRPHY_DIAG) == 1);
 assert(count(&f, HUD1541_EVENT_HDRMETA_DIAG) == 1);
 assert(f.rpm_valid && f.rpm_track == 18);
 assert(f.mailbox.events[1] & (1u << 27));
 for (unsigned bit = 0; bit < 8; ++bit) {
  init(&f, HUD1541_TELEM_ALL);
  header(&f, 0xFC ^ (1u << bit));
  assert(count(&f, HUD1541_EVENT_HDRPHY_DIAG) == 0);
  assert(count(&f, HUD1541_EVENT_HDRMETA_DIAG) == 1);
  assert(!(f.mailbox.events[0] & (1u << 27)));
  assert(!f.rpm_valid);
  header(&f, 0xFC);
  assert(count(&f, HUD1541_EVENT_HDRPHY_DIAG) == 1);
  assert(f.rpm_valid);
 }
 init(&f, HUD1541_TELEM_ALL);
 for (unsigned i = 0; i < 16; ++i) header(&f, 0xFD);
 assert(count(&f, HUD1541_EVENT_HDRPHY_DIAG) == 0);
 assert(count(&f, HUD1541_EVENT_HDRMETA_DIAG) == 2);
 assert(!f.mailbox.event_overflow);
 init(&f, HUD1541_TELEM_METADATA);
 header(&f, 0xFC);
 assert(count(&f, HUD1541_EVENT_HDRPHY_DIAG) == 0);
 assert(count(&f, HUD1541_EVENT_HDRMETA_DIAG) == 1);
 init(&f, HUD1541_TELEM_ALL);
 /* DOS pre-search order must not become a physical header. */
 write_ram(&f, RAM_HDRID2, 0x5A); write_ram(&f, RAM_HDRID1, 0xA5);
 write_ram(&f, RAM_HDRTRK, 18); write_ram(&f, RAM_HDRSEC, 17);
 write_ram(&f, RAM_HDRCHK, 0xFC);
 assert(!f.mailbox.event_head && !f.rpm_valid);
 puts("Production decoder: valid/bad checksums, recovery, diagnostic sampling and subscription passed");
}
