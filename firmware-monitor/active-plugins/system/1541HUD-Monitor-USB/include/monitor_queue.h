#ifndef HUD_MONITOR_QUEUE_H
#define HUD_MONITOR_QUEUE_H
#include <stdbool.h>
#include <stdint.h>
// Single-core queue: all producers and the USB drain run in the task loop.
typedef struct {
    uint8_t *data;
    uint16_t capacity, head, tail;
    uint32_t waits, oversize;
} hud_tx_t;
static inline uint32_t hud_tx_used(const hud_tx_t *q) {
    return (q->head + q->capacity - q->tail) % q->capacity;
}
static inline bool hud_tx_enqueue(hud_tx_t *q, const char *data, uint32_t n) {
    if (n >= q->capacity) { q->oversize++; return false; }
    if (n > q->capacity - 1u - hud_tx_used(q)) { q->waits++; return false; }
    for (uint32_t i=0; i<n; i++) {
        q->data[q->head]=(uint8_t)data[i];
        q->head=(uint16_t)((q->head+1u)%q->capacity);
    }
    return true;
}
static inline uint32_t hud_tx_peek(hud_tx_t *q, const uint8_t **data) {
    uint32_t n=q->capacity-q->tail, used=hud_tx_used(q);
    if (n>used) n=used;
    *data=q->data+q->tail;
    return n;
}
static inline bool hud_tx_consume(hud_tx_t *q, uint32_t n) {
    if (n>hud_tx_used(q)) return false;
    q->tail=(uint16_t)((q->tail+n)%q->capacity);
    return true;
}
#endif
