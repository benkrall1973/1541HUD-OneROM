#ifndef HUD1541_MAILBOX_H
#define HUD1541_MAILBOX_H

#include <stdint.h>
#include <stddef.h>

#define HUD1541_MAILBOX_ADDR 0x20081C00u
#define HUD1541_MAILBOX_MAGIC 0x33414844u
#define HUD1541_MAILBOX_VERSION 30u

#define HUD1541_FLAG_INPUTS_SAFE (1u << 0)
#define HUD1541_FLAG_PIO_RUNNING (1u << 1)
#define HUD1541_FLAG_DMA_RUNNING (1u << 2)
#define HUD1541_FLAG_CAPTURE_VALID (1u << 3)
#define HUD1541_FLAG_EVENT_OVERFLOW (1u << 4)
#define HUD1541_FLAG_RING_PRESSURE (1u << 5)
#define HUD1541_FLAG_RING_OVERRUN (1u << 6)
#define HUD1541_FLAG_UNSAFE_OUTPUT (1u << 31)

#define HUD1541_RING_WORDS 64u
#define HUD1541_EVENT_WORDS 32u

#define HUD1541_EVENT_MOTOR 1u
#define HUD1541_EVENT_PHASE 2u
#define HUD1541_EVENT_TRACK_WRITE 3u
#define HUD1541_EVENT_WRITE_PROTECT 4u
#define HUD1541_EVENT_DENSITY 5u

typedef struct {
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

 volatile uint32_t events[HUD1541_EVENT_WORDS];

 /* RP2350 current-state cache. Layout remains identical to the hardware-proven
  * V0.0.30 mailbox so the acquisition and USB transport semantics do not move.
  */
 volatile uint32_t track_state;
 volatile uint32_t last_drvst;

 volatile uint32_t ring[HUD1541_RING_WORDS];
} hud1541_mailbox_t;

#define HUD1541_MAILBOX \
 ((volatile hud1541_mailbox_t *)(uintptr_t)HUD1541_MAILBOX_ADDR)

typedef char hud1541_mailbox_size_must_be_512[
 (sizeof(hud1541_mailbox_t) == 512u) ? 1 : -1
];

typedef char hud1541_ring_offset_must_be_0x100[
 (offsetof(hud1541_mailbox_t, ring) == 0x100u) ? 1 : -1
];

#endif
