#ifndef DRIVEHUD_MAILBOX_H
#define DRIVEHUD_MAILBOX_H

#include <stdint.h>
#include <stddef.h>

#define DRIVEHUD_MAILBOX_ADDR 0x20081C00u
#define DRIVEHUD_MAILBOX_MAGIC 0x33414844u
#define DRIVEHUD_MAILBOX_VERSION 30u

#define DRIVEHUD_FLAG_INPUTS_SAFE (1u << 0)
#define DRIVEHUD_FLAG_PIO_RUNNING (1u << 1)
#define DRIVEHUD_FLAG_DMA_RUNNING (1u << 2)
#define DRIVEHUD_FLAG_CAPTURE_VALID (1u << 3)
#define DRIVEHUD_FLAG_EVENT_OVERFLOW (1u << 4)
#define DRIVEHUD_FLAG_RING_PRESSURE (1u << 5)
#define DRIVEHUD_FLAG_RING_OVERRUN (1u << 6)
#define DRIVEHUD_FLAG_UNSAFE_OUTPUT (1u << 31)

#define DRIVEHUD_RING_WORDS 64u
#define DRIVEHUD_EVENT_WORDS 32u

#define DRIVEHUD_EVENT_MOTOR 1u
#define DRIVEHUD_EVENT_PHASE 2u
#define DRIVEHUD_EVENT_TRACK_WRITE 3u
#define DRIVEHUD_EVENT_WRITE_PROTECT 4u
#define DRIVEHUD_EVENT_DENSITY 5u

typedef struct {
 /* 30 metadata words. */
 volatile uint32_t magic;
 volatile uint32_t version;
 volatile uint32_t flags;
 volatile uint32_t capture_count;
 volatile uint32_t consumed_count;
 volatile uint32_t event_head;
 volatile uint32_t event_tail;
 volatile uint32_t event_overflow;
 volatile uint32_t ring_pressure;
 volatile uint32_t last_orb;
 volatile uint32_t last_phase;
 volatile uint32_t last_motor;
 volatile uint32_t home_count;
 volatile uint32_t pio_pc;
 volatile uint32_t rx_level;
 volatile uint32_t dma_write_addr;
 volatile uint32_t consumer_index;
 volatile uint32_t producer_index;
 volatile uint32_t dma_busy;
 volatile uint32_t dma_trans_count;
 volatile uint32_t produced_total;
 volatile uint32_t consumer_total;
 volatile uint32_t ring_overrun;
 volatile uint32_t phase_event_count;
 volatile uint32_t last_write_protect;
 volatile uint32_t write_protect_event_count;
 volatile uint32_t write_protect_valid;
 volatile uint32_t last_density;
 volatile uint32_t density_event_count;
 volatile uint32_t density_valid;

 volatile uint32_t events[DRIVEHUD_EVENT_WORDS];

 /* RP2350 current-state cache.
  *
  * track_state:
  *   bits  0..7   last DOS $0022 write
  *   bit      8   last DOS $0022 valid
  *   bits 16..23  current physical position in half-track units (pos2)
  *   bit     24   current physical position valid
  *   bit     25   DOS DRVST says head stepping
  *
  * last_drvst holds the latest DOS $0020 value.
  *
  * These reuse V28's two words, so the DMA ring remains at +0x100.
  */
 volatile uint32_t track_state;
 volatile uint32_t last_drvst;

 /* 64 words = 256-byte DMA ring. */
 volatile uint32_t ring[DRIVEHUD_RING_WORDS];
} drivehud_mailbox_t;

#define DRIVEHUD_MAILBOX \
 ((volatile drivehud_mailbox_t *)(uintptr_t)DRIVEHUD_MAILBOX_ADDR)

typedef char drivehud_mailbox_size_must_be_512[
 (sizeof(drivehud_mailbox_t) == 512u) ? 1 : -1
];

typedef char drivehud_ring_offset_must_be_0x100[
 (offsetof(drivehud_mailbox_t, ring) == 0x100u) ? 1 : -1
];

#endif
