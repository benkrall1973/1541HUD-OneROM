// DriveHUD UB4 passive monitor v0.0.17
// PIO write-filter + RP2350 endless circular DMA test.
// Based on DH11. DH12 ping-pong architecture is not used.

#include "plugin.h"
#include "drivehud_mailbox.h"

ORA_DEFINE_USER_PLUGIN(drivehud_probe_main, 0,0,17,0, 0,7,1);

#define RESETS_BASE 0x40020000u
#define RESET_RESET (*(volatile uint32_t *)(RESETS_BASE+0x00u))
#define RESET_DONE  (*(volatile uint32_t *)(RESETS_BASE+0x08u))
#define RESET_DMA  (1u<<2)
#define RESET_PIO0 (1u<<11)

#define PIO0_BASE 0x50200000u
#define PIO_CTRL          (*(volatile uint32_t *)(PIO0_BASE+0x000u))
#define PIO_FLEVEL        (*(volatile uint32_t *)(PIO0_BASE+0x00Cu))
#define PIO_RXF3          (*(volatile uint32_t *)(PIO0_BASE+0x02Cu))
#define PIO_INSTR_MEM(n)  (*(volatile uint32_t *)(PIO0_BASE+0x048u+((n)*4u)))
#define PIO_SM3_CLKDIV    (*(volatile uint32_t *)(PIO0_BASE+0x110u))
#define PIO_SM3_EXECCTRL  (*(volatile uint32_t *)(PIO0_BASE+0x114u))
#define PIO_SM3_SHIFTCTRL (*(volatile uint32_t *)(PIO0_BASE+0x118u))
#define PIO_SM3_ADDR      (*(volatile uint32_t *)(PIO0_BASE+0x11Cu))
#define PIO_SM3_INSTR     (*(volatile uint32_t *)(PIO0_BASE+0x120u))
#define PIO_SM3_PINCTRL   (*(volatile uint32_t *)(PIO0_BASE+0x124u))

#define DMA_BASE 0x50000000u
#define DMA_CH 11u
typedef struct {
    volatile uint32_t read_addr;
    volatile uint32_t write_addr;
    volatile uint32_t transfer_count;
    volatile uint32_t ctrl_trig;
} dma_ch_t;
#define DMA11 ((dma_ch_t *)(DMA_BASE + DMA_CH*0x40u))

/* RP2350 DMA CTRL_TRIG fields */
#define DMA_EN            (1u<<0)
#define DMA_SIZE_32       (2u<<2)
#define DMA_INCR_WRITE    (1u<<6)
#define DMA_RING_SIZE(n)  (((n)&0xFu)<<8)
#define DMA_RING_SEL      (1u<<12)
#define DMA_CHAIN_TO(x)   (((x)&0xFu)<<13)
#define DMA_TREQ(x)       (((x)&0x3Fu)<<17)
#define DMA_IRQ_QUIET     (1u<<23)
#define DMA_BUSY          (1u<<24)
#define DREQ_PIO0_RX3 7u

#define MASK_CS1  (1u<<24)
#define MASK_nCS2 (1u<<25)

/* Fire-24-E mapping */
static uint8_t logical_data_from_gpio(uint32_t v) {
    return (uint8_t)((((v >> 7) & 1u) << 0) |
                     (((v >> 6) & 1u) << 1) |
                     (((v >> 5) & 1u) << 2) |
                     (((v >> 0) & 1u) << 3) |
                     (((v >> 1) & 1u) << 4) |
                     (((v >> 2) & 1u) << 5) |
                     (((v >> 3) & 1u) << 6) |
                     (((v >> 4) & 1u) << 7));
}

static uint32_t logical_addr13_from_gpio(uint32_t v) {
    return (((v >> 23) & 1u) << 0)  |
           (((v >> 22) & 1u) << 1)  |
           (((v >> 21) & 1u) << 2)  |
           (((v >> 20) & 1u) << 3)  |
           (((v >> 19) & 1u) << 4)  |
           (((v >> 18) & 1u) << 5)  |
           (((v >> 17) & 1u) << 6)  |
           (((v >> 16) & 1u) << 7)  |
           (((v >> 15) & 1u) << 8)  |
           (((v >> 14) & 1u) << 9)  |
           (((v >> 13) & 1u) << 10) |
           (((v >> 11) & 1u) << 11) |
           (((v >> 12) & 1u) << 12);
}

static uint8_t uc2_selected(uint32_t v) {
    return (uint8_t)(((v & MASK_CS1) != 0u) && ((v & MASK_nCS2) == 0u));
}

/* PIO already removed all READ cycles, so decoder only sees writes. */
static uint8_t uc2_reg0_write(uint32_t v) {
    return (uint8_t)(uc2_selected(v) &&
                     ((logical_addr13_from_gpio(v) & 0x0Fu) == 0u));
}

static uint8_t ram_write_0022(uint32_t v) {
    return (uint8_t)(!uc2_selected(v) &&
                     (logical_addr13_from_gpio(v) == 0x0022u));
}

static uint8_t all_inputs(ora_gpio_query_fn_t q) {
    const uint8_t pins[] = {
        0,1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
    };
    uint32_t i;
    for (i=0; i<(uint32_t)sizeof(pins); ++i) {
        ora_gpio_info_t info;
        info.size = sizeof(info);
        if (q(pins[i], &info) != ORA_RESULT_OK || info.is_output) return 0;
    }
    return 1;
}

static void queue_event(volatile drivehud_mailbox_t *m, uint32_t word) {
    uint32_t head = m->event_head;
    uint32_t next = (head + 1u) % DRIVEHUD_EVENT_WORDS;
    if (next == m->event_tail) {
        m->event_overflow++;
        m->flags |= DRIVEHUD_FLAG_EVENT_OVERFLOW;
        return;
    }
    m->events[head] = word;
    __asm volatile("dmb sy" ::: "memory");
    m->event_head = next;
}

static void queue_motor(volatile drivehud_mailbox_t *m, uint8_t state) {
    queue_event(m, (DRIVEHUD_EVENT_MOTOR << 28) | (uint32_t)(state & 1u));
}

static void queue_phase(volatile drivehud_mailbox_t *m,
                        uint8_t oldp, uint8_t newp, uint8_t delta, uint8_t motor) {
    uint32_t w = (DRIVEHUD_EVENT_PHASE << 28) |
                 ((uint32_t)(oldp & 3u) << 0) |
                 ((uint32_t)(newp & 3u) << 2) |
                 ((uint32_t)(delta & 3u) << 4) |
                 ((uint32_t)(motor & 1u) << 6);
    queue_event(m, w);
}

static void queue_track(volatile drivehud_mailbox_t *m, uint8_t data) {
    queue_event(m, (DRIVEHUD_EVENT_TRACK_WRITE << 28) | (uint32_t)data);
}

static void pio_dma_init(volatile drivehud_mailbox_t *m) {
    RESET_RESET &= ~(RESET_PIO0 | RESET_DMA);
    while ((RESET_DONE & (RESET_PIO0 | RESET_DMA)) != (RESET_PIO0 | RESET_DMA)) {}

    PIO_CTRL &= ~(1u<<3);
    PIO_CTRL |=  (1u<<(4+3));

    /*
     * PIO program at 26..31.
     *
     * 26 WAIT 1 GPIO8       PHI2 rising
     * 27 JMP 28 [20]        same DH11 T22/MID delay
     * 28 JMP PIN,31         GPIO9 R/W: high=READ, skip capture
     * 29 IN PINS,32         WRITE cycle snapshot
     * 30 WAIT 0 GPIO8
     * 31 WAIT 0 GPIO8       common read/write end path
     * wrap -> 26
     */
    PIO_INSTR_MEM(26) = 0x2088u;
    PIO_INSTR_MEM(27) = 0x141Cu;
    PIO_INSTR_MEM(28) = 0x00DFu;
    PIO_INSTR_MEM(29) = 0x4000u;
    PIO_INSTR_MEM(30) = 0x2008u;
    PIO_INSTR_MEM(31) = 0x2008u;

    PIO_SM3_CLKDIV = (1u<<16);
    PIO_SM3_EXECCTRL = (31u<<12) | (26u<<7) | (9u<<24);
    PIO_SM3_SHIFTCTRL = (1u<<16);
    PIO_SM3_PINCTRL = 0u;
    PIO_SM3_INSTR = 0x001Au; /* execute JMP 26 */

    DMA11->ctrl_trig = 0u;
    DMA11->read_addr = (uint32_t)(uintptr_t)&PIO_RXF3;
    DMA11->write_addr = (uint32_t)(uintptr_t)&m->ring[0];

    /*
     * DH16 uses RP2350 TRANS_COUNT MODE=1 (TRIGGER_SELF) with the maximum
     * 28-bit finite count.  Unlike ENDLESS mode, the live low 28 bits now
     * decrement once per completed DMA transfer.  When they reach zero the
     * channel immediately re-triggers itself and reloads the count, while the
     * ring write address continues from its current position.
     *
     * This gives software an independent producer-progress clock, eliminating
     * DH14's ambiguous "write_addr modulo 64" producer tracking.  A full 64
     * word lap can no longer masquerade as producer==consumer.
     *
     * MODE=1 in bits 31:28, COUNT=0x0fffffff.
     * 256-byte ring: RING_SIZE=8, RING_SEL=write.
     */
    DMA11->transfer_count = 0x1fffffffu;
    DMA11->ctrl_trig =
        DMA_EN |
        DMA_SIZE_32 |
        DMA_INCR_WRITE |
        DMA_RING_SIZE(8u) |
        DMA_RING_SEL |
        DMA_CHAIN_TO(DMA_CH) |
        DMA_TREQ(DREQ_PIO0_RX3) |
        DMA_IRQ_QUIET;

    PIO_CTRL |= (1u<<3);

    m->flags |= DRIVEHUD_FLAG_PIO_RUNNING | DRIVEHUD_FLAG_DMA_RUNNING;
}

void drivehud_probe_main(ora_lookup_fn_t lookup,
                         ora_plugin_type_t type,
                         const ora_entry_args_t *args) {
    ora_gpio_query_fn_t gpio_query;
    volatile drivehud_mailbox_t *m = DRIVEHUD_MAILBOX;
    uint32_t i;
    uint32_t consumer_total = 0u;
    uint32_t produced_total = 0u;
    uint32_t last_remaining = 0x0fffffffu;
    uint8_t phase_valid = 0u, motor_valid = 0u;
    uint8_t last_phase = 0u, last_motor = 0u;

    (void)args;
    gpio_query = (ora_gpio_query_fn_t)lookup(ORA_ID_GPIO_QUERY);

    m->magic = DRIVEHUD_MAILBOX_MAGIC;
    m->version = DRIVEHUD_MAILBOX_VERSION;
    m->flags = 0u;
    m->capture_count = 0u;
    m->consumed_count = 0u;
    m->event_head = 0u;
    m->event_tail = 0u;
    m->event_overflow = 0u;
    m->ring_pressure = 0u;
    m->last_orb = 0u;
    m->last_phase = 0u;
    m->last_motor = 0u;
    m->home_count = 0u;
    m->pio_pc = 0u;
    m->rx_level = 0u;
    m->dma_write_addr = 0u;
    m->consumer_index = 0u;
    m->producer_index = 0u;
    m->dma_busy = 0u;
    m->dma_trans_count = 0u;
    m->produced_total = 0u;
    m->consumer_total = 0u;
    m->ring_overrun = 0u;
    m->phase_event_count = 0u;

    for (i=0; i<DRIVEHUD_EVENT_WORDS; ++i) m->events[i] = 0u;
    for (i=0; i<DRIVEHUD_RING_WORDS; ++i) m->ring[i] = 0u;

    if (type != ORA_PLUGIN_TYPE_USER || gpio_query == 0 || !all_inputs(gpio_query)) {
        m->flags = DRIVEHUD_FLAG_UNSAFE_OUTPUT;
        while (1) __asm volatile("wfi");
    }

    m->flags = DRIVEHUD_FLAG_INPUTS_SAFE;
    pio_dma_init(m);

    while (1) {
        const uint32_t dma_reload = 0x0fffffffu;
        uint32_t wa;
        uint32_t remaining;
        uint32_t advanced;
        uint32_t available;

        /*
         * TRANS_COUNT low 28 bits are a real decrementing counter in
         * TRIGGER_SELF mode.  Accumulate completed transfers across automatic
         * reloads.  Because reload happens only after 0x0fffffff transfers,
         * this loop would have to be stalled for minutes before more than one
         * reload could pass unnoticed.
         */
        remaining = DMA11->transfer_count & 0x0fffffffu;
        if (remaining <= last_remaining) {
            advanced = last_remaining - remaining;
        } else {
            /* Counter reloaded between observations. */
            advanced = last_remaining + (dma_reload - remaining);
        }
        produced_total += advanced;
        last_remaining = remaining;

        m->pio_pc = PIO_SM3_ADDR & 0x1Fu;
        m->rx_level = (PIO_FLEVEL >> 28) & 0xFu;
        wa = DMA11->write_addr;
        m->dma_write_addr = wa;
        m->consumer_index = consumer_total & (DRIVEHUD_RING_WORDS - 1u);
        m->producer_index = produced_total & (DRIVEHUD_RING_WORDS - 1u);
        m->dma_busy = (DMA11->ctrl_trig & DMA_BUSY) ? 1u : 0u;
        m->dma_trans_count = DMA11->transfer_count;
        m->produced_total = produced_total;
        m->consumer_total = consumer_total;

        /*
         * With an absolute producer count we can finally detect a true DMA
         * ring overrun.  DH14 could not distinguish an empty ring from a
         * producer that had lapped the consumer by exactly 64 entries.
         */
        available = produced_total - consumer_total;
        if (available > DRIVEHUD_RING_WORDS) {
            uint32_t lost = available - DRIVEHUD_RING_WORDS;
            m->ring_overrun += lost;
            m->ring_pressure++;
            m->flags |= DRIVEHUD_FLAG_RING_PRESSURE;
            consumer_total = produced_total - DRIVEHUD_RING_WORDS;
            available = DRIVEHUD_RING_WORDS;
        } else if (available >= 48u) {
            m->ring_pressure++;
            m->flags |= DRIVEHUD_FLAG_RING_PRESSURE;
        }

        while (consumer_total != produced_total) {
            uint32_t idx = consumer_total & (DRIVEHUD_RING_WORDS - 1u);
            uint32_t s = m->ring[idx];
            consumer_total++;
            m->capture_count++;
            m->consumed_count++;
            m->consumer_total = consumer_total;
            m->flags |= DRIVEHUD_FLAG_CAPTURE_VALID;

            if (uc2_reg0_write(s)) {
                uint8_t orb = logical_data_from_gpio(s);
                uint8_t motor = (uint8_t)((orb >> 2) & 1u);
                uint8_t phase = (uint8_t)(orb & 3u);

                m->last_orb = orb;
                m->last_phase = phase;
                m->last_motor = motor;

                if (!motor_valid) {
                    motor_valid = 1u;
                    last_motor = motor;
                    queue_motor(m, motor);
                } else if (motor != last_motor) {
                    last_motor = motor;
                    queue_motor(m, motor);
                }

                if (!phase_valid) {
                    phase_valid = 1u;
                    last_phase = phase;
                } else if (phase != last_phase) {
                    uint8_t oldp = last_phase;
                    uint8_t delta = (uint8_t)((phase - oldp) & 3u);
                    last_phase = phase;
                    m->phase_event_count++;
                    queue_phase(m, oldp, phase, delta, motor);
                }
            }

            if (ram_write_0022(s)) {
                uint8_t data = logical_data_from_gpio(s);

                /*
                 * V0.5.16 only gives physical meaning to $0022=1: HOME.
                 * Do not consume event-queue space for all other DOS writes.
                 */
                if (data == 1u) {
                    m->home_count++;
                    queue_track(m, data);
                }
            }

            /*
             * Refresh the decrementing DMA counter while draining.  This keeps
             * producer_total current without relying on modulo write_addr.
             */
            remaining = DMA11->transfer_count & 0x0fffffffu;
            if (remaining <= last_remaining) {
                advanced = last_remaining - remaining;
            } else {
                advanced = last_remaining + (dma_reload - remaining);
            }
            produced_total += advanced;
            last_remaining = remaining;
            m->produced_total = produced_total;

            available = produced_total - consumer_total;
            if (available > DRIVEHUD_RING_WORDS) {
                uint32_t lost = available - DRIVEHUD_RING_WORDS;
                m->ring_overrun += lost;
                m->ring_pressure++;
                m->flags |= DRIVEHUD_FLAG_RING_PRESSURE;
                consumer_total = produced_total - DRIVEHUD_RING_WORDS;
                m->consumer_total = consumer_total;
            } else if (available >= 48u) {
                m->ring_pressure++;
                m->flags |= DRIVEHUD_FLAG_RING_PRESSURE;
            }
        }
    }
}
