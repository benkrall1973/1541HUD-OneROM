#ifndef DRIVEHUD_MAILBOX_H
#define DRIVEHUD_MAILBOX_H

#include <stdint.h>

#define DRIVEHUD_MAILBOX_ADDR 0x20081C00u
#define DRIVEHUD_MAILBOX_MAGIC 0x33414844u
#define DRIVEHUD_MAILBOX_VERSION 17u

#define DRIVEHUD_FLAG_INPUTS_SAFE    (1u << 0)
#define DRIVEHUD_FLAG_PIO_RUNNING    (1u << 1)
#define DRIVEHUD_FLAG_DMA_RUNNING    (1u << 2)
#define DRIVEHUD_FLAG_CAPTURE_VALID  (1u << 3)
#define DRIVEHUD_FLAG_EVENT_OVERFLOW (1u << 4)
#define DRIVEHUD_FLAG_RING_PRESSURE  (1u << 5)
#define DRIVEHUD_FLAG_UNSAFE_OUTPUT  (1u << 31)

#define DRIVEHUD_RING_WORDS 64u
#define DRIVEHUD_EVENT_WORDS 32u

#define DRIVEHUD_EVENT_MOTOR       1u
#define DRIVEHUD_EVENT_PHASE       2u
#define DRIVEHUD_EVENT_TRACK_WRITE 3u

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

    volatile uint32_t events[DRIVEHUD_EVENT_WORDS];

    /* 24 metadata + 32 events + 8 pad = 64 words = 0x100 bytes */
    volatile uint32_t reserved[8];

    /* 256-byte aligned DMA ring; USER data ends exactly at 0x20081E00 */
    volatile uint32_t ring[DRIVEHUD_RING_WORDS];
} drivehud_mailbox_t;

#define DRIVEHUD_MAILBOX ((volatile drivehud_mailbox_t *)(uintptr_t)DRIVEHUD_MAILBOX_ADDR)

#endif
