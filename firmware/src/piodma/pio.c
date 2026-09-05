// Copyright (C) 2026 Piers Finlayson <piers@piers.rocks>
//
// MIT License

// RP2350 Shared PIO routines

#include "include.h"

#if defined(TEST_BUILD)
#define TEST_PIO_C
#else
#define APIO_LOG_IMPL  1
#endif // TEST_BUILD

#include "piodma/piodma.h"

int pio(void) {
    int rc;

#if defined(HUD1541_PASSIVE_UB4)
    // 1541HUD UB4 is a passive monitor. setup_initial_gpios() has already
    // placed every GPIO in input-only mode (except board system LED pins).
    // Do NOT call piorom2(): that would configure and enable the normal ROM
    // serving state machines and could drive the 1541 data bus.
    // Returning here lets vector.c launch the configured plugins while all
    // ROM-socket/address/select/X GPIOs remain inputs.
    return 0;
#endif

    if (0) {
        DEBUG("PIO RAM Mode");
        uint32_t rom_table_addr = (uint32_t)(uintptr_t)RUNTIME->rom_table;
        rc = pioram(INFO, RUNTIME, rom_table_addr);
    } else {
        DEBUG("PIO ROM Mode");
        rc = piorom2();
    }

    return rc;
}
