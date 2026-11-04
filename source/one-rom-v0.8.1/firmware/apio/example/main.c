// MIT License
//
// Copyright (c) 2026 Piers Finlayson <piers@piers.rocks>
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

// apio - Runtime RP2350 PIO Assembler and Disassembler
//
// Example using apio to create a simple PIO program that toggles GPIO0.  It
// decides, at runtime, how long to pause for between toggles based on a
// random bit from the RP2350's ring oscillator.

// Use RTT for logging
#include <SEGGER_RTT.h>
#define APIO_LOG_IMPL  1
#define APIO_LOG_ENABLE(...) do { \
                                SEGGER_RTT_printf(0, __VA_ARGS__); \
                                SEGGER_RTT_printf(0, "\n"); \
                            } while(0)

// Include the apio assembler headers
#include <apio.h>

// Random number generator register definitions
#define RNG_VALID   (*(volatile uint32_t *)0x400F0110 & 1)
#define RANDOM_BIT  (*(volatile uint32_t *)0x400F0114 & 1)

int main() {
    // Enable JTAG/SWD for logging
    APIO_ENABLE_JTAG();

    // Global system configuration for PIO usage
    APIO_ENABLE_GPIOS();    // Bring GPIOs out of reset
    APIO_ENABLE_PIOS();     // Bring PIOs out of reset
    APIO_GPIO_INPUT_OUTPUT(0, 0); // Configure GPIO0 as controllable output by PIO block 0

    // PIO assembler initialization - must be called before using any other
    // assembler macros, and must be in the same function as those macros.
    APIO_ASM_INIT();        // Initialize assembler
    APIO_CLEAR_ALL_IRQS();  // Clear any pending PIO IRQs

    // PIO block and state machine selection
    APIO_SET_BLOCK(0);      // Select PIO block 0
    APIO_SET_SM(0);         // Select state machine 0

    // Choose between two delay durations based on the RP2350's TRNG
    while (!RNG_VALID);     // Wait for RNG to be valid
    uint8_t pause_count = RANDOM_BIT ? 15 : 30;

    // PIO0 SM0 program
    APIO_ADD_INSTR(APIO_SET_PIN_DIRS(1));   // Set pin as output
    APIO_WRAP_BOTTOM();                     // Set .wrap_bottom to current instruction address
    APIO_ADD_INSTR(APIO_ADD_DELAY(APIO_SET_PINS(1), pause_count)); // Drive pin high, and pause
    APIO_WRAP_TOP();                        // Set .wrap_top to current instruction address
    APIO_ADD_INSTR(APIO_ADD_DELAY(APIO_SET_PINS(0), pause_count)); // Drive pin low, and pause

    // Configure PIO0 SM0
    APIO_SM_CLKDIV_SET(15000, 0);   // Set clock divider so runs at 0.01 MHz (150 MHz / 15000)
                                    // although in this example the clock hasn't been set
                                    // up, so is running from ring oscillator.
                                    // Using the ring oscillator, the PIN toggles at ~10Hz
    APIO_SM_EXECCTRL_SET(0);        // No EXECCTRL settings enabled
    APIO_SM_SHIFTCTRL_SET(0);       // No SHIFTCTRL settings required
    APIO_SM_PINCTRL_SET(            // One output pin starting at GPIO 0
        APIO_SET_BASE(0) |
        APIO_SET_COUNT(1)
    );
    APIO_SM_JMP_TO_START();         // Confiure SM to jump to start of program

    // Log SM configuration if API_LOG_ENABLE is defined
    APIO_LOG_SM("Example SM");

    // Writes PIO block 0 instructions to hardware
    APIO_END_BLOCK();

    // Start the PIO SM
    APIO_ENABLE_SMS(0, (1 << 0)); // Enable PIO0 SM0

    // Main loop - does nothing, PIO runs independently in the background
    while (1) {
        APIO_ASM_WFI(); // Wait for interrupt (or just return if in emulation)
    }
}
