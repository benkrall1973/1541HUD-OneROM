// 1541 UB3 ROM Selector test extension for OneROM USB system plugin.
// Frozen interface used by USB Selector v1.0.0 and later IEC test builds.
#ifndef USB_ROM_SELECT_H
#define USB_ROM_SELECT_H

#include <stddef.h>
#include <stdint.h>

void romsel_init(void);
void romsel_cdc_rx(const uint8_t *buf, uint32_t count);
void romsel_task(void);

#endif
