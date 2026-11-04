// Copyright (C) 2026 Piers Finlayson <piers@piers.rocks>
//
// MIT License

// apio - Runtime RP2350 PIO Assembler and Disassembler
//
// Macros to construct PIO programs for RP2350 PIO state machines.

#ifndef APIO_H
#define APIO_H

//
// Macros to build PIO programs
//

// Instructions:
//
// You MUST build all SMs for a single PIO block and write them using
// `APIO_END_BLOCK()` before moving onto the next PIO block, as a single
// stack based scratch buffer is used by these macros.
//
// 1.  At the beginning of your PIO building function, call `APIO_ASM_INIT()`
//     to declare and initialise the necessary variables.
//
//     If the function is extending an already-configured PIO setup (e.g.
//     adding state machines to a block that was already written by an earlier
//     call), use `APIO_ASM_CONTINUE()` instead.  See its documentation below.
//
// 2.  (Optional) Clear all PIO IRQs using `APIO_CLEAR_ALL_IRQS()`.
//
// 3.  Start the first block with `APIO_SET_BLOCK(BLOCK_NUM)` where `BLOCK_NUM`
//     is 0, 1 or 2.
//
// 4.  Starting with SM 0, start the first program with `APIO_SET_SM(SM_NUM)`
//     where `SM_NUM` is 0, 1, 2 or 3.
//
// 5.  (Optional) Create any labels required before the next instruction using
//     `APIO_LABEL_NEW(LABEL_NAME)`, where `LABEL_NAME` must be unique within
//     your function.  These labels are used as destinations for JMP
//     instructions.
//
// 6.  (Optional) Use `APIO_START()`, `APIO_WRAP_BOTTOM()`, `APIO_WRAP_TOP()` and
//     `APIO_END()` before the instruction, to mark the start and wrap points of
//     your program.
// 
//     `APIO_WRAP_TOP()` must be called _before_ adding the instruction that is
//     to be the wrap top.  You do not need to call these macros if the
//     .start, .wrap_bottom or .wrap_top are to be at instruction 0.
//
//     `APIO_END()` is only required if the program ends beyond .wrap, and, if
//     used, must be called _after_ `APIO_WRAP_TOP()`, but before the final
//     instruction of the program.
//
// 7.  Add PIO instructions using `APIO_ADD_INSTR(INSTRUCTION)`.
//
// 8.  Repeat steps 5 to 7 for this SM's program.
//
// 9.  Call 'APIO_SM_CLKDIV_SET(INT, FRAC)` to set the SM's clock divider.
//
// 10. Call `APIO_SM_EXECCTRL_SET(EXECCTRL)` to set the SM's EXECCTRL register.
//     There is no need to encode the wrap top and bottom here, as they are
//     handled automatically.
//
// 11. Call `APIO_SM_SHIFTCTRL_SET(SHIFTCTRL)` to set the SM's SHIFTCTRL
//     register.
//
// 12. Call `APIO_SM_PINCTRL_SET(PINCTRL)` to set the SM's PINCTRL register.
//
// 13. (Optional) Use `APIO_SM_INSTR_SET(INSTRUCTION)` to execute discrete
//     instructions on this SM immediately after configuration.
//
// 14. Call `APIO_SM_JMP_TO_START()` to set the SM to jump to the start of the
//     program after configuration.
//
// 15. (Optional) Call `APIO_LOG_SM("SM NAME")` to log the SM program details
//     for debugging.
//
// 16. (Optional) Repeat steps 4 to 15 for each additional SM in this PIO block.
//
// 17. Call `APIO_END_BLOCK()` to write all constructed programs to the PIO
//     instruction memory.
//
// 18. Repeat steps 3 to 17 for each additional PIO block.

#include <stdint.h>
#include <apio_reg.h>

#define APIO_MAX_PIO_INSTRS      32
#define APIO_MAX_SMS_PER_BLOCK   4
#define APIO_MAX_PIO_BLOCKS      3
#define APIO_MAX_FIFO_DEPTH      4
#define APIO_MAX_GPIOS           48

// Bitmask with all APIO_MAX_GPIOS bits set (for default pull-down init)
#define APIO_GPIO_ALL_MASK  ((1ULL << APIO_MAX_GPIOS) - 1)

#if defined(APIO_EMULATION)
#define MAX_PRE_INSTRS   16
typedef struct {
    uint32_t irq[APIO_MAX_PIO_BLOCKS];
    uint8_t first_instr[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint8_t start[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint8_t end[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint8_t wrap_bottom[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint8_t wrap_top[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    pio_sm_reg_t pio_sm_reg[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint16_t instr[APIO_MAX_PIO_BLOCKS][APIO_MAX_PIO_INSTRS];
    uint16_t pre_instr[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK][MAX_PRE_INSTRS];
    uint8_t pre_instr_count[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint32_t tx_fifos[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK][APIO_MAX_FIFO_DEPTH];
    uint8_t tx_fifo_count[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint32_t rx_fifos[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK][APIO_MAX_FIFO_DEPTH];
    uint8_t rx_fifo_count[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint8_t offset[APIO_MAX_PIO_BLOCKS];
    uint8_t max_offset[APIO_MAX_PIO_BLOCKS];
    uint8_t enabled_sms[APIO_MAX_PIO_BLOCKS];
    uint8_t block;
    uint8_t sm;
    uint8_t block_ended[APIO_MAX_PIO_BLOCKS];
    uint8_t pios_enabled;
    uint32_t gpio_base[APIO_MAX_PIO_BLOCKS];

    // Members are appended here rather than inserted above.  This structure is
    // defined by whichever consumer compiles it, and read by every other, so
    // an insertion moves every member below it and a consumer built against a
    // different version of this header reads them all at the wrong offset.
    // Appending confines the disagreement to the members it does not have.

    // Words a full TX FIFO discarded, per SM.  A device raises FDEBUG_TXOVER
    // and drops the word, so a caller that overfills loses data either way -
    // this is what lets a test see that it happened.
    uint8_t tx_fifo_overflow[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];

    // Where a discarded word goes, so APIO_TXF stays an assignable expression
    // whether or not the FIFO has room.  Never read.
    uint32_t tx_discard;
} _apio_emulated_pio_t;

typedef struct {
    int8_t  output_block[APIO_MAX_GPIOS];
    uint8_t inverted[APIO_MAX_GPIOS];
    uint8_t force_input_low[APIO_MAX_GPIOS];
    uint8_t force_input_high[APIO_MAX_GPIOS];
    // Pull resistor configuration.  Hardware reset default: pull-down.
    uint64_t pull_up;
    uint64_t pull_down;
    // Input-only: output driver disabled.
    uint64_t input_only;
    // Drive strength per pin (APIO_DRIVE_* values).  Hardware reset default:
    // APIO_DRIVE_4MA.
    uint8_t drive_strength[APIO_MAX_GPIOS];
    // Slew rate.  Hardware reset default: slow (0).
    uint64_t slew_fast;
} _apio_emulated_gpio_t;

extern _apio_emulated_pio_t _apio_emulated_pio;
extern _apio_emulated_gpio_t _apio_emulated_gpios;
#define __blk  _apio_emulated_pio.block
#define __sm   _apio_emulated_pio.sm
#define __pio_start   _apio_emulated_pio.start
#define __pio_wrap_bottom   _apio_emulated_pio.wrap_bottom
#define __pio_wrap_top   _apio_emulated_pio.wrap_top
#define __pio_end   _apio_emulated_pio.end
#define __pio_offset _apio_emulated_pio.offset
#define __pio_first_instr _apio_emulated_pio.first_instr
#undef APIO0_SM_REG
#define APIO0_SM_REG(SM)  (&_apio_emulated_pio.pio_sm_reg[__blk][SM])
#undef APIO1_SM_REG
#define APIO1_SM_REG(SM)  (&_apio_emulated_pio.pio_sm_reg[__blk][SM])
#undef APIO2_SM_REG
#define APIO2_SM_REG(SM)  (&_apio_emulated_pio.pio_sm_reg[__blk][SM])
#undef APIO0_GPIOBASE
#define APIO0_GPIOBASE _apio_emulated_pio.gpio_base[0] 
#undef APIO1_GPIOBASE 
#define APIO1_GPIOBASE _apio_emulated_pio.gpio_base[1] 
#undef APIO2_GPIOBASE
#define APIO2_GPIOBASE _apio_emulated_pio.gpio_base[2]
#if defined(APIO_EMU_IMPL)
_apio_emulated_pio_t _apio_emulated_pio = {
    .irq = {0xFFFFFFFF},
    .first_instr = {{0xFF}},
    .start = {{0xFF}},
    .end = {{0xFF}},
    .wrap_bottom = {{0xFF}},
    .wrap_top = {{0xFF}},
    .pio_sm_reg = {{{
        .clkdiv = 0xFFFFFFFF,
        .execctrl = 0xFFFFFFFF,
        .shiftctrl = 0xFFFFFFFF,
        .pinctrl = 0xFFFFFFFF
    }}},
    .instr = {{0xFFFF}},
    .pre_instr = {{{0xFFFF}}},
    .pre_instr_count = {{0xFF}},
    .tx_fifos = {{{0xFFFFFFFF}}},
    .tx_fifo_count = {{0xFF}},
    .rx_fifos = {{{0xFFFFFFFF}}},
    .rx_fifo_count = {{0xFF}},
    .offset = {0xFF},
    .max_offset = {0xFF},
    .enabled_sms = {0xFF},
    .block = 0xFF,
    .sm = 0xFF,
    .block_ended = {0xFF},
    .pios_enabled = 0xFF,
    .gpio_base = {0xFFFFFFFF},
    .tx_fifo_overflow = {{0xFF}},
    .tx_discard = 0xFFFFFFFF
};
// GPIO defaults match RP2350 hardware reset state:
//   pull-down enabled, 4mA drive strength, slow slew.
_apio_emulated_gpio_t _apio_emulated_gpios = {
    .output_block    = {[0 ... 47] = -1},
    .inverted        = {0},
    .force_input_low = {0},
    .force_input_high= {0},
    .pull_up         = 0,
    .pull_down       = 0x0000FFFFFFFFFFFFULL,  // bits 0-47 set
    .input_only      = 0,
    .drive_strength  = {[0 ... 47] = APIO_DRIVE_4MA},
    .slew_fast       = 0,
};
#endif // APIO_EMU_IMPL
#endif // APIO_EMULATION

// Include disassembler here to allow APIOX_SM_REG to be redefined for
// emulation
#include <apio_dis.h>

// Internal macros - do not use directly
#define _STATIC_BLOCK_ASSERT(BLOCK) _Static_assert((BLOCK) >= 0 && (BLOCK) < APIO_MAX_PIO_BLOCKS, "Invalid PIO block")

// Internal macro - do not use directly
#define _STATIC_SM_ASSERT(SM)       _Static_assert((SM) >= 0 && (SM) < APIO_MAX_SMS_PER_BLOCK, "Invalid PIO state machine")

// Internal macro - do not use directly
#define _OFFSET_ARRAY_INIT       {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}

// Macro to bring JTAG/SWD out of reset, for SWD logging
#if !defined(APIO_EMULATION)
#define APIO_ENABLE_JTAG() do { \
                                APIO_RESET_RESET &= ~APIO_RESET_JTAG; \
                                while (!(APIO_RESET_DONE & APIO_RESET_JTAG)); \
                            } while(0)
#else // APIO_EMULATION
#define APIO_ENABLE_JTAG()
#endif // !APIO_EMULATION

// Macro to bring IOBANK0 and PADS_BANK0 out of reset, allowing GPIO usage.
#if !defined(APIO_EMULATION)
#define APIO_ENABLE_GPIOS() do { \
                                APIO_RESET_RESET &= ~(APIO_RESET_IOBANK0 | APIO_RESET_PADS_BANK0); \
                                while (!(APIO_RESET_DONE & (APIO_RESET_IOBANK0 | APIO_RESET_PADS_BANK0))); \
                            } while(0)
#else // APIO_EMULATION
#define APIO_ENABLE_GPIOS()
#endif // !APIO_EMULATION

// Configure a GPIO for input/output usage by a PIO block.  Assigns the GPIO
// function to the specified PIO block and enables the output driver.  The
// input path remains available.
#if !defined(APIO_EMULATION)
_Static_assert(APIO_GPIO_CTRL_FUNC_PIO1 == (APIO_GPIO_CTRL_FUNC_PIO0 + 1), "APIO_GPIO_CTRL_FUNC_PIO1 must be APIO_GPIO_CTRL_FUNC_PIO0 + 1");
_Static_assert(APIO_GPIO_CTRL_FUNC_PIO2 == (APIO_GPIO_CTRL_FUNC_PIO0 + 2), "APIO_GPIO_CTRL_FUNC_PIO2 must be APIO_GPIO_CTRL_FUNC_PIO0 + 2");
#define APIO_GPIO_INPUT_OUTPUT(PIN, BLOCK) \
                            do { \
                                _STATIC_BLOCK_ASSERT(BLOCK); \
                                APIO_GPIO_CTRL(PIN) = APIO_GPIO_CTRL_FUNC_PIO0 + (BLOCK); \
                                APIO_GPIO_PAD(PIN) &= ~(APIO_PAD_ISO_BIT | APIO_PAD_OUTPUT_DIS_BIT); \
                                APIO_GPIO_PAD(PIN) |=  APIO_PAD_INPUT_EN_BIT; \
                            } while(0)
#else // APIO_EMULATION
#define APIO_GPIO_INPUT_OUTPUT(PIN, BLOCK) do { \
                                _STATIC_BLOCK_ASSERT(BLOCK); \
                                _apio_emulated_gpios.output_block[PIN] = BLOCK; \
                                _apio_emulated_gpios.input_only &= ~(1ULL << (PIN)); \
                            } while(0)
#endif // !APIO_EMULATION
// Retain APIO_GPIO_OUTPUT for backwards compatibility
#define APIO_GPIO_OUTPUT APIO_GPIO_INPUT_OUTPUT

// Configure a GPIO as a pure SIO input with output driver disabled.
// On real hardware: resets GPIO_CTRL to SIO function, sets PAD to
// input-enabled, output-disabled.
// In emulation: marks the pin input-only; EPIO will assert on any attempt
// to drive it as an output.
#if !defined(APIO_EMULATION)
#define APIO_GPIO_INPUT_ONLY(PIN) \
                            do { \
                                APIO_GPIO_CTRL(PIN) = APIO_GPIO_CTRL_FUNC_SIO; \
                                APIO_GPIO_PAD(PIN) = APIO_PAD_INPUT_EN_BIT | APIO_PAD_OUTPUT_DIS_BIT; \
                            } while(0)
#else // APIO_EMULATION
#define APIO_GPIO_INPUT_ONLY(PIN) do { \
                                _apio_emulated_gpios.output_block[PIN] = -1; \
                                _apio_emulated_gpios.input_only |= (1ULL << (PIN)); \
                            } while(0)
#endif // !APIO_EMULATION

// Configure a pull-up resistor on a GPIO.  Clears any pull-down.
#if !defined(APIO_EMULATION)
#define APIO_GPIO_PULL_UP(PIN) \
                            do { \
                                APIO_GPIO_PAD(PIN) &= ~APIO_PAD_PDE_BIT; \
                                APIO_GPIO_PAD(PIN) |=  APIO_PAD_PUE_BIT; \
                            } while(0)
#else // APIO_EMULATION
#define APIO_GPIO_PULL_UP(PIN) do { \
                                _apio_emulated_gpios.pull_up   |=  (1ULL << (PIN)); \
                                _apio_emulated_gpios.pull_down &= ~(1ULL << (PIN)); \
                            } while(0)
#endif // !APIO_EMULATION

// Configure a pull-down resistor on a GPIO.  Clears any pull-up.
#if !defined(APIO_EMULATION)
#define APIO_GPIO_PULL_DOWN(PIN) \
                            do { \
                                APIO_GPIO_PAD(PIN) &= ~APIO_PAD_PUE_BIT; \
                                APIO_GPIO_PAD(PIN) |=  APIO_PAD_PDE_BIT; \
                            } while(0)
#else // APIO_EMULATION
#define APIO_GPIO_PULL_DOWN(PIN) do { \
                                _apio_emulated_gpios.pull_down |=  (1ULL << (PIN)); \
                                _apio_emulated_gpios.pull_up   &= ~(1ULL << (PIN)); \
                            } while(0)
#endif // !APIO_EMULATION

// Disable both pull-up and pull-down resistors on a GPIO.
#if !defined(APIO_EMULATION)
#define APIO_GPIO_PULL_NONE(PIN) \
                            do { \
                                APIO_GPIO_PAD(PIN) &= ~(APIO_PAD_PUE_BIT | APIO_PAD_PDE_BIT); \
                            } while(0)
#else // APIO_EMULATION
#define APIO_GPIO_PULL_NONE(PIN) do { \
                                _apio_emulated_gpios.pull_up   &= ~(1ULL << (PIN)); \
                                _apio_emulated_gpios.pull_down &= ~(1ULL << (PIN)); \
                            } while(0)
#endif // !APIO_EMULATION

// Set the drive strength of a GPIO output.  STRENGTH must be one of
// APIO_DRIVE_2MA, APIO_DRIVE_4MA, APIO_DRIVE_8MA, APIO_DRIVE_12MA.
// Hardware reset default: APIO_DRIVE_4MA.
#if !defined(APIO_EMULATION)
#define APIO_GPIO_DRIVE(PIN, STRENGTH) \
                            do { \
                                APIO_GPIO_PAD(PIN) &= ~APIO_PAD_DRIVE_MASK; \
                                APIO_GPIO_PAD(PIN) |=  APIO_PAD_DRIVE(STRENGTH); \
                            } while(0)
#else // APIO_EMULATION
#define APIO_GPIO_DRIVE(PIN, STRENGTH) do { \
                                _apio_emulated_gpios.drive_strength[PIN] = (STRENGTH); \
                            } while(0)
#endif // !APIO_EMULATION

// Set the slew rate of a GPIO output to fast.
// Hardware reset default: slow.
#if !defined(APIO_EMULATION)
#define APIO_GPIO_SLEW_FAST(PIN) \
                            do { \
                                APIO_GPIO_PAD(PIN) |= APIO_PAD_SLEWFAST_BIT; \
                            } while(0)
#else // APIO_EMULATION
#define APIO_GPIO_SLEW_FAST(PIN) do { \
                                _apio_emulated_gpios.slew_fast |= (1ULL << (PIN)); \
                            } while(0)
#endif // !APIO_EMULATION

// Set the slew rate of a GPIO output to slow (hardware reset default).
#if !defined(APIO_EMULATION)
#define APIO_GPIO_SLEW_SLOW(PIN) \
                            do { \
                                APIO_GPIO_PAD(PIN) &= ~APIO_PAD_SLEWFAST_BIT; \
                            } while(0)
#else // APIO_EMULATION
#define APIO_GPIO_SLEW_SLOW(PIN) do { \
                                _apio_emulated_gpios.slew_fast &= ~(1ULL << (PIN)); \
                            } while(0)
#endif // !APIO_EMULATION

// Invert a GPIO input
#if !defined(APIO_EMULATION)
#define APIO_GPIO_INPUT_INVERT(PIN) do { \
                                    APIO_GPIO_CTRL(PIN) &= ~APIO_GPIO_CTRL_INOVER_MASK; \
                                    APIO_GPIO_CTRL(PIN) |= APIO_GPIO_CTRL_INOVER_INVERT; \
                                } while(0)
#define APIO_GPIO_FORCE_INPUT_LOW(PIN)  do { \
                                    APIO_GPIO_CTRL(PIN) &= ~APIO_GPIO_CTRL_INOVER_MASK; \
                                    APIO_GPIO_CTRL(PIN) |= APIO_GPIO_CTRL_INOVER_LOW; \
                                } while(0)
#define APIO_GPIO_FORCE_INPUT_HIGH(PIN)  do { \
                                    APIO_GPIO_CTRL(PIN) &= ~APIO_GPIO_CTRL_INOVER_MASK; \
                                    APIO_GPIO_CTRL(PIN) |= APIO_GPIO_CTRL_INOVER_HIGH; \
                                } while(0)
#else // APIO_EMULATION
#define APIO_GPIO_INPUT_INVERT(PIN) do { \
                                    _apio_emulated_gpios.inverted[PIN] = 1; \
                                    _apio_emulated_gpios.force_input_high[PIN] = 0; \
                                    _apio_emulated_gpios.force_input_low[PIN] = 0; \
                                } while(0)
#define APIO_GPIO_FORCE_INPUT_LOW(PIN)  do { \
                                    _apio_emulated_gpios.inverted[PIN] = 0; \
                                    _apio_emulated_gpios.force_input_high[PIN] = 0; \
                                    _apio_emulated_gpios.force_input_low[PIN] = 1; \
                                } while(0)
#define APIO_GPIO_FORCE_INPUT_HIGH(PIN)  do { \
                                    _apio_emulated_gpios.inverted[PIN] = 0; \
                                    _apio_emulated_gpios.force_input_low[PIN] = 0; \
                                    _apio_emulated_gpios.force_input_high[PIN] = 1; \
                                } while(0)
#endif // !APIO_EMULATION

// Clears IRQs for the specified PIO block
#if !defined(APIO_EMULATION)
#define PIO_CLEAR_IRQ(BLOCK)    _STATIC_BLOCK_ASSERT(BLOCK); \
                                if (BLOCK == 0) {           \
                                    APIO0_IRQ = 0xFFFFFFFF;  \
                                } else if (BLOCK == 1) {    \
                                    APIO1_IRQ = 0xFFFFFFFF;  \
                                } else {                    \
                                    APIO2_IRQ = 0xFFFFFFFF;  \
                                }
#else // APIO_EMULATION
#define PIO_CLEAR_IRQ(BLOCK)    _STATIC_BLOCK_ASSERT(BLOCK); \
                                _apio_emulated_pio.irq[BLOCK] = 0xFFFFFFFF
#endif // !APIO_EMULATION

// Clear all PIO IRQs
#if !defined(APIO_EMULATION)
#define APIO_CLEAR_ALL_IRQS()   APIO0_IRQ = 0xFFFFFFFF;  \
                                APIO1_IRQ = 0xFFFFFFFF;  \
                                APIO2_IRQ = 0xFFFFFFFF
#else // APIO_EMULATION
#define APIO_CLEAR_ALL_IRQS()   for (int __i = 0; __i < APIO_MAX_PIO_BLOCKS; __i++) { \
                                    _apio_emulated_pio.irq[__i] = 0xFFFFFFFF; \
                                }
#endif // !APIO_EMULATION

// Call at the start of a function that builds PIO programs from scratch.
// Declares the local variables used by all subsequent APIO macros in this
// function scope.
//
// In emulation mode, resets the accumulated apio state (preserving only
// pios_enabled) so the new build starts clean.
//
// Use APIO_ASM_CONTINUE() instead when the function extends an
// already-configured PIO setup (e.g. adding state machines to a block that
// was already written by an earlier call).
//
// Uses around 128 bytes of stack space in RP2350 hardware case.
#if !defined(APIO_EMULATION)
#define APIO_ASM_INIT()  \
    uint16_t __attribute__((unused)) instr_scratch[APIO_MAX_PIO_INSTRS]; \
    uint8_t __attribute__((unused)) __pio_first_instr[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK] = _OFFSET_ARRAY_INIT; \
    uint8_t __attribute__((unused)) __pio_start[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK] = _OFFSET_ARRAY_INIT; \
    uint8_t __attribute__((unused)) __pio_wrap_bottom[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK] = _OFFSET_ARRAY_INIT; \
    uint8_t __attribute__((unused)) __pio_wrap_top[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK] = _OFFSET_ARRAY_INIT; \
    uint8_t __attribute__((unused)) __pio_end[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK] = _OFFSET_ARRAY_INIT; \
    uint8_t __attribute__((unused)) __pio_offset[APIO_MAX_PIO_BLOCKS] = {0, 0, 0}; \
    uint8_t __blk = 0; \
    uint8_t __sm = 0
#else // APIO_EMULATION
#define APIO_ASM_INIT() uint8_t _pios_enabled = _apio_emulated_pio.pios_enabled; \
                        _apio_emulated_pio = (_apio_emulated_pio_t){0};                    \
                        _apio_emulated_pio.pios_enabled = _pios_enabled
#endif // !APIO_EMULATION

// Call at the start of a function that extends an already-configured PIO
// setup — i.e. one that appends new state machines to a block already written
// by an earlier APIO_ASM_INIT() call, or that issues APIO_SM_EXEC_INSTR
// against a live SM without touching the program memory at all.
//
// In non-emulation mode this delegates to APIO_ASM_INIT(), which declares
// the local variables required by all APIO macros.  The block offset is then
// positioned correctly by the caller via APIO_SET_BLOCK_FROM() or
// APIO_SET_BLOCK_FROM_VAR().
//
// In emulation mode this is a deliberate no-op: __blk, __sm, __pio_offset
// and the per-SM tracking arrays are all macros over the global
// _apio_emulated_pio, so they are already in scope and already hold the
// accumulated state from all prior APIO_ASM_INIT() / APIO_ASM_CONTINUE()
// calls.  Preserving that state is exactly what allows epio_update_from_apio()
// to pick up new programs, SM configs, pre_instrs, and tx_fifos written by
// this function without disturbing the configuration laid down earlier.
#if !defined(APIO_EMULATION)
// In non-emulation mode, APIO_ASM_CONTINUE() is identical to APIO_ASM_INIT():
// the local variables required by all APIO macros must still be declared, and
// the block offset is positioned correctly by the subsequent
// APIO_SET_BLOCK_FROM() or APIO_SET_BLOCK_FROM_VAR() call.
#define APIO_ASM_CONTINUE()  APIO_ASM_INIT()
#else // APIO_EMULATION
#define APIO_ASM_CONTINUE() /* no-op in emulation: accumulated apio state is preserved */
#endif // !APIO_EMULATION

// Call before using APIO GPIO macros.  Resets all GPIO configuration to
// hardware reset defaults: pull-down, 4mA drive strength, slow slew.
#if !defined(APIO_EMULATION)
#define APIO_GPIO_INIT()
#else // APIO_EMULATION
#define APIO_GPIO_INIT() do { \
                            for (int __i = 0; __i < APIO_MAX_GPIOS; __i++) { \
                                _apio_emulated_gpios.output_block[__i]   = -1; \
                                _apio_emulated_gpios.inverted[__i]        = 0; \
                                _apio_emulated_gpios.force_input_low[__i] = 0; \
                                _apio_emulated_gpios.force_input_high[__i]= 0; \
                                _apio_emulated_gpios.drive_strength[__i]  = APIO_DRIVE_4MA; \
                            } \
                            _apio_emulated_gpios.pull_up    = 0; \
                            _apio_emulated_gpios.pull_down  = APIO_GPIO_ALL_MASK; \
                            _apio_emulated_gpios.input_only = 0; \
                            _apio_emulated_gpios.slew_fast  = 0; \
                        } while(0)
#endif // !APIO_EMULATION

// Assert these, as if they change, the above stack space calculation must be updated.
_Static_assert((APIO_MAX_PIO_BLOCKS == 3), "APIO_MAX_PIO_BLOCKS must be 3");
_Static_assert((APIO_MAX_SMS_PER_BLOCK == 4), "APIO_MAX_SMS_PER_BLOCK must be 4");
_Static_assert((APIO_MAX_PIO_INSTRS == 32), "APIO_MAX_PIO_INSTRS must be 32");

// Bring PIO blocks out of reset
#if !defined(APIO_EMULATION)
#define APIO_ENABLE_PIOS()  do { \
                                APIO_RESET_RESET &= ~(APIO_RESET_PIO0 | APIO_RESET_PIO1 | APIO_RESET_PIO2 ); \
                                while (!(APIO_RESET_DONE & (APIO_RESET_PIO0 | APIO_RESET_PIO1 | APIO_RESET_PIO2))); \
                            } while(0)
#else // APIO_EMULATION
#define APIO_ENABLE_PIOS()    _apio_emulated_pio.pios_enabled = 1
#endif // !APIO_EMULATION

#if !defined(APIO_EMULATION)
#define APIO_ENABLE_SMS(BLOCK, SMS_MASK)  \
                                _STATIC_BLOCK_ASSERT(BLOCK); \
                                _Static_assert((SMS_MASK) > 0 && (SMS_MASK) < (1 << APIO_MAX_SMS_PER_BLOCK), "Invalid SMS_MASK"); \
                                if (BLOCK == 0) {           \
                                    APIO0_CTRL_SM_ENABLE(SMS_MASK);  \
                                } else if (BLOCK == 1) {    \
                                    APIO1_CTRL_SM_ENABLE(SMS_MASK);  \
                                } else {                    \
                                    APIO2_CTRL_SM_ENABLE(SMS_MASK);  \
                                }
#else // APIO_EMULATION
#define APIO_ENABLE_SMS(BLOCK, SMS_MASK)    _apio_emulated_pio.enabled_sms[BLOCK] = SMS_MASK
#endif // !APIO_EMULATION

// Set the current PIO block where the block number is a runtime variable
#define APIO_SET_BLOCK_VAR(BLOCK)               __blk = (BLOCK)

// Set the current PIO block
#define APIO_SET_BLOCK(BLOCK)                   _STATIC_BLOCK_ASSERT(BLOCK); \
                                                APIO_SET_BLOCK_VAR(BLOCK)

// Resume building a previously committed block from a known instruction
// offset, where the block number is a runtime variable
#define APIO_SET_BLOCK_FROM_VAR(BLOCK, OFFSET)  APIO_SET_BLOCK_VAR(BLOCK); \
                                                __pio_offset[__blk] = (OFFSET)

// Resume building a previously committed block from a known instruction offset
#define APIO_SET_BLOCK_FROM(BLOCK, OFFSET)      _STATIC_BLOCK_ASSERT(BLOCK); \
                                                APIO_SET_BLOCK_FROM_VAR(BLOCK, OFFSET)

// Set the current PIO SM using an SM variable
#define APIO_SET_SM_VAR(SM)     __sm = (SM);                                            \
                                __pio_first_instr[__blk][__sm] = __pio_offset[__blk];   \
                                __pio_start[__blk][__sm] = __pio_offset[__blk];         \
                                __pio_wrap_bottom[__blk][__sm] = __pio_offset[__blk];   \
                                __pio_wrap_top[__blk][__sm] = __pio_offset[__blk];      \
                                __pio_end[__blk][__sm] = __pio_offset[__blk]

// Set the current PIO SM
#define APIO_SET_SM(SM)         _STATIC_SM_ASSERT(SM); \
                                APIO_SET_SM_VAR(SM)

// Use a label as a destination for JMPs
#define APIO_LABEL(NAME)        __pio_label__##NAME

// Create a label for JMPs
#define APIO_LABEL_NEW(NAME)    uint8_t APIO_LABEL(NAME) = __pio_offset[__blk]

// Create a label for JMPs at a relative offset
#define APIO_LABEL_NEW_OFFSET(NAME, OFFSET) uint8_t APIO_LABEL(NAME) = __pio_offset[__blk] + (OFFSET)

// Set the start offset within a PIO program - call before `APIO_ADD_INSTR()`
// for the start instruction.
#define APIO_START()            __pio_start[__blk][__sm] = __pio_offset[__blk]

// Get a label representing the start of the current PIO program
#define APIO_START_LABEL()      __pio_start[__blk][__sm]

// Returns the number of instructions stored for the current PIO block.
#define APIO_INSTR_COUNT()  (__pio_offset[__blk])

// Set the end offset within a PIO program - call before `APIO_ADD_INSTR()`
// for the last instruction.  Must be called after `APIO_WRAP_TOP()`.  If
// .wrap is the last instruction, this is not required.
#define APIO_END()              __pio_end[__blk][__sm] = __pio_offset[__blk]

// Set the wrap bottom offset within a PIO program - call before
// `APIO_ADD_INSTR()` for the .wrap_target instruction.
#define APIO_WRAP_BOTTOM()      __pio_wrap_bottom[__blk][__sm] = __pio_offset[__blk]

// Set the wrap top offset within a PIO program - call before
// `APIO_ADD_INSTR()` for the .wrap instruction.
#define APIO_WRAP_TOP()         __pio_wrap_top[__blk][__sm] = __pio_offset[__blk];  \
                                APIO_END()

// Add an instruction to the current PIO program.
#if !defined(APIO_EMULATION)
#define APIO_ADD_INSTR(INST)    instr_scratch[__pio_offset[__blk]++] = INST
#else // APIO_EMULATION
#define APIO_ADD_INSTR(INST)    _apio_emulated_pio.instr[__blk][_apio_emulated_pio.offset[__blk]++] = INST
#endif // !APIO_EMULATION

// Set the clock divider for the current PIO SM.
#define APIO_SM_CLKDIV_SET(INT, FRAC)   _apio_sm_reg_ptr(__blk, __sm)->clkdiv = APIO_CLKDIV((INT), (FRAC))

// Set the EXECCTRL for the current PIO SM.  Do not include wrap top/bottom.
// Those will be set automatically from the wrap values.
#define APIO_SM_EXECCTRL_SET(EXECCTRL)  _apio_sm_reg_ptr(__blk, __sm)->execctrl =  \
                                            (EXECCTRL) | \
                                            APIO_WRAP_BOTTOM_AS_REG(__pio_wrap_bottom[__blk][__sm]) |  \
                                            APIO_WRAP_TOP_AS_REG(__pio_wrap_top[__blk][__sm])

// Set the SHIFTCTRL for the current PIO SM.
#define APIO_SM_SHIFTCTRL_SET(SHIFTCTRL)    _apio_sm_reg_ptr(__blk, __sm)->shiftctrl = (SHIFTCTRL)

// Set the PINCTRL for the current PIO SM.
#define APIO_SM_PINCTRL_SET(PINCTRL)    _apio_sm_reg_ptr(__blk, __sm)->pinctrl = (PINCTRL)

static inline volatile pio_sm_reg_t* _apio_sm_reg_ptr(uint8_t block, uint8_t sm) {
    if (block == 0) return APIO0_SM_REG(sm);
    else if (block == 1) return APIO1_SM_REG(sm);
    else return APIO2_SM_REG(sm);
}

// Immediately execute an instruction on the current PIO SM.  Can be called
// before enabling the SM to set initial state.
#if !defined(APIO_EMULATION)
#define APIO_SM_EXEC_INSTR(INSTR) _apio_sm_reg_ptr(__blk, __sm)->instr = INSTR
#else // APIO_EMULATION
#define APIO_SM_EXEC_INSTR(INSTR) _apio_emulated_pio.pre_instr[__blk][__sm][_apio_emulated_pio.pre_instr_count[__blk][__sm]++] = INSTR;
#endif // !APIO_EMULATION

#if !defined(APIO_EMULATION)
static inline volatile uint32_t* _apio_txf_ptr(uint8_t block, uint8_t sm) {
    if (block == 0) return (volatile uint32_t *)((uintptr_t)APIO0_BASE + APIO_TXF_OFFSET + (sm * 0x04));
    else if (block == 1) return (volatile uint32_t *)((uintptr_t)APIO1_BASE + APIO_TXF_OFFSET + (sm * 0x04));
    else return (volatile uint32_t *)((uintptr_t)APIO2_BASE + APIO_TXF_OFFSET + (sm * 0x04));
}
static inline volatile uint32_t* _apio_rxf_ptr(uint8_t block, uint8_t sm) {
    if (block == 0) return (volatile uint32_t *)((uintptr_t)APIO0_BASE + APIO_RXF_OFFSET + (sm * 0x04));
    else if (block == 1) return (volatile uint32_t *)((uintptr_t)APIO1_BASE + APIO_RXF_OFFSET + (sm * 0x04));
    else return (volatile uint32_t *)((uintptr_t)APIO2_BASE + APIO_RXF_OFFSET + (sm * 0x04));
}
// Pops a word the state machine pushed.  Yields a value rather than the
// register itself, so a write through APIO_RXF does not compile.
static inline uint32_t _apio_rxf_read(uint8_t block, uint8_t sm) {
    return *_apio_rxf_ptr(block, sm);
}
#endif // !APIO_EMULATION

#if defined(APIO_EMULATION)
// Where the next word written to an SM's TX FIFO goes.
//
// A device's FIFO is four words deep and discards anything written to a full
// one, keeping what is already queued.  This does the same.  Past the depth
// the word lands in a sink nothing reads, and the discard is counted, so a
// test can assert it.
static inline uint32_t *_apio_emu_txf_ptr(uint8_t block, uint8_t sm) {
    uint8_t *count = &_apio_emulated_pio.tx_fifo_count[block][sm];

    if (*count >= APIO_MAX_FIFO_DEPTH) {
        _apio_emulated_pio.tx_fifo_overflow[block][sm]++;
        return &_apio_emulated_pio.tx_discard;
    }

    return &_apio_emulated_pio.tx_fifos[block][sm][(*count)++];
}

// The word an SM's RX FIFO holds next, or zero where it holds nothing.
//
// The emulated FIFO is filled by the emulator running the program, not from
// here, so this reads and does not pop.  An SM's output is a property of the
// run, and a test reading it twice should see the same thing.
static inline uint32_t _apio_emu_rxf(uint8_t block, uint8_t sm) {
    if (_apio_emulated_pio.rx_fifo_count[block][sm] == 0) {
        return 0;
    }

    return _apio_emulated_pio.rx_fifos[block][sm][0];
}
#endif // APIO_EMULATION

// Write a named SM's TX FIFO, without needing the assembler's block and SM
// variables in scope.
//
// APIO_TXF reaches only the SM the assembler is currently building, and
// getting there means APIO_SET_SM, which resets that SM's program bookkeeping.
// That is right while building a program and wrong for code that feeds a
// running state machine, which is what this is for.
#if !defined(APIO_EMULATION)
#define APIO_TXF_AT(BLOCK, SM)  (*_apio_txf_ptr((BLOCK), (SM)))
#else // APIO_EMULATION
#define APIO_TXF_AT(BLOCK, SM)  (*_apio_emu_txf_ptr((BLOCK), (SM)))
#endif // !APIO_EMULATION

// Read a named SM's RX FIFO.  Not assignable - see APIO_RXF.
#if !defined(APIO_EMULATION)
#define APIO_RXF_AT(BLOCK, SM)  _apio_rxf_read((BLOCK), (SM))
#else // APIO_EMULATION
#define APIO_RXF_AT(BLOCK, SM)  _apio_emu_rxf((BLOCK), (SM))
#endif // !APIO_EMULATION

// Access the current SM's TX FIFO.
#define APIO_TXF  APIO_TXF_AT(__blk, __sm)

// Read the current SM's RX FIFO.
//
// A read, and only a read.  RXFn is read-only to the processor, so this yields
// a value rather than an lvalue and a write through it does not compile.
//
// To place a word in an emulated RX FIFO, tell the emulator - epio exposes
// epio_push_rx_fifo() for exactly that.
#define APIO_RXF  APIO_RXF_AT(__blk, __sm)

// Set the current PIO SM to jump to its start instruction after
// configuration.  The PIO SM will only be started by explicitly enabling.
// This sets the point at which it will start.
#define APIO_SM_JMP_TO_START()  APIO_SM_EXEC_INSTR(APIO_JMP(__pio_start[__blk][__sm]))

#if !defined(APIO_EMULATION)
static inline volatile uint32_t* _apio_instr_mem_ptr(uint8_t block) {
    if (block == 0) return (volatile uint32_t *)(APIO0_BASE + APIO_INSTR_MEM_OFFSET);
    else if (block == 1) return (volatile uint32_t *)(APIO1_BASE + APIO_INSTR_MEM_OFFSET);
    else return (volatile uint32_t *)(APIO2_BASE + APIO_INSTR_MEM_OFFSET);
}
#endif // !APIO_EMULATION

// Commits only the instructions added since APIO_SET_BLOCK_FROM(), leaving
// previously committed instructions in PIO memory untouched.
#if !defined(APIO_EMULATION)
#define APIO_END_BLOCK_FROM(OFFSET) do { \
                            volatile uint32_t* ptr = _apio_instr_mem_ptr(__blk);        \
                            for (int ii = (OFFSET); ii < __pio_offset[__blk]; ii++) {   \
                                ptr[ii] = instr_scratch[ii];                            \
                            }                                                           \
                        } while(0)
#else
#define APIO_END_BLOCK_FROM(OFFSET) _apio_emulated_pio.max_offset[__blk] = __pio_offset[__blk]
#endif

// Write the constructed PIO programs to the PIO instruction memory for the
// current PIO block.  Call after all SMs for this block have been built,
// before enabling.
#define APIO_END_BLOCK() APIO_END_BLOCK_FROM(0)

// Call to enable one or more SMs within a PIO block.  To enable more than SM
// simultaneously, OR the SM numbers together (e.g. to enable SM0 and SM2, use
// 0b00000101 = 5).
#if !defined(APIO_EMULATION)
#define APIO_ENABLE_SM(BLOCK, SM_MASK)  _STATIC_BLOCK_ASSERT(BLOCK);         \
                                        _Static_assert((SM_MASK < 0xF), "Attempt to enable invalid SM"); \
                                        if (BLOCK == 0) {                   \
                                            APIO0_CTRL_SM_ENABLE(SM_MASK);   \
                                        } else if (BLOCK == 1) {            \
                                            APIO1_CTRL_SM_ENABLE(SM_MASK);   \
                                        } else {                            \
                                            APIO2_CTRL_SM_ENABLE(SM_MASK);   \
                                        }
#else // APIO_EMULATION
#define APIO_ENABLE_SM(BLOCK, SM_MASK)  _STATIC_BLOCK_ASSERT(BLOCK);         \
                                        _Static_assert((SM_MASK < 0xF), "Attempt to enable invalid SM"); \
                                        _apio_emulated_pio.enabled_sms[BLOCK] |= SM_MASK
#endif // !APIO_EMULATION

// Set GPIOBASE to 0 for the current PIO block
#define APIO_GPIOBASE_0()   if (__blk == 0) {                 \
                                APIO0_GPIOBASE = APIO_GPIOBASE_VAL_0; \
                            } else if (__blk == 1) {          \
                                APIO1_GPIOBASE = APIO_GPIOBASE_VAL_0; \
                            } else {                            \
                                APIO2_GPIOBASE = APIO_GPIOBASE_VAL_0; \
                            }

// Set GPIOBASE to 16 for the current PIO block
#define APIO_GPIOBASE_16()  if (__blk == 0) {                     \
                                APIO0_GPIOBASE = APIO_GPIOBASE_VAL_16;    \
                            } else if (__blk == 1) {              \
                                APIO1_GPIOBASE = APIO_GPIOBASE_VAL_16;    \
                            } else {                                \
                                APIO2_GPIOBASE = APIO_GPIOBASE_VAL_16;    \
                            }

//
// PIO Instruction Macros
//

// Add a side set delay from 0-31 cycles to an instruction
#define APIO_ADD_DELAY(INST, DELAY)  ((INST) | (((DELAY) & 0x1F) << 8))

// Apply invert (bitwise NOT) to a MOV instruction's source
#define APIO_MOV_SRC_INVERT(INST)     ((INST) | (0b01 << 3))

// Apply bit-reverse to a MOV instruction's source
#define APIO_MOV_SRC_REVERSE(INST)    ((INST) | (0b10 << 3))

// Move the pins value to the ISR
#define APIO_IN_PINS(NUM)            (0x4000 | ((NUM) & 0x1F))

// Move X to the ISR
#define APIO_IN_X(NUM)               (0x4020 | ((NUM) & 0x1F))

// Move Y to the ISR
#define APIO_IN_Y(NUM)               (0x4040 | ((NUM) & 0x1F))

// Move null (zeros) to the ISR
#define APIO_IN_NULL(NUM)            (0x4060 | ((NUM) & 0x1F))

// Move ISR to ISR
#define APIO_IN_ISR(NUM)             (0x40C0 | ((NUM) & 0x1F))

// Move OSR to ISR
#define APIO_IN_OSR(NUM)             (0x40E0 | ((NUM) & 0x1F))

// Clear one of this PIO block's IRQs
#define APIO_IRQ_CLEAR(X)            (0xC040 | ((X) & 0x07))

// Clear one of the previous PIO block's IRQs
#define APIO_IRQ_CLEAR_PREV(X)       (0xC048 | ((X) & 0x07))

// Clear one of the next PIO block's IRQs
#define APIO_IRQ_CLEAR_NEXT(X)       (0xC058 | ((X) & 0x07))

// Clear one of the PIO block's IRQs using relative addressing
#define APIO_IRQ_CLEAR_REL(X)        (0xC050 | ((X) & 0x07))

// Set one of this PIO block's IRQs to 1
#define APIO_IRQ_SET(X)              (0xC000 | ((X) & 0x07))

// Set one of the previous PIO block's IRQs to 1
#define APIO_IRQ_SET_PREV(X)         (0xC008 | ((X) & 0x07))

// Set one of the next PIO block's IRQs to 1
#define APIO_IRQ_SET_NEXT(X)         (0xC018 | ((X) & 0x07))

// Set on the PIO block's IRQs to 1 using relative addressing
#define APIO_IRQ_SET_REL(X)          (0xC010 | ((X) & 0x07))

// Set one of this PIO block's IRQs to 1 and then wait for it to be cleared
#define APIO_IRQ_SET_WAIT(X)          (0xC020 | ((X) & 0x07))

// Set one of the previous PIO block's IRQs to 1 and then wait for it to be
// cleared
#define APIO_IRQ_SET_WAIT_PREV(X)     (0xC028 | ((X) & 0x07))

// Set one of the next PIO block's IRQs to 1 and then wait for it to be
// cleared
#define APIO_IRQ_SET_WAIT_NEXT(X)     (0xC038 | ((X) & 0x07))

// Set one of the PIO block's IRQs to 1 using relative addressing and then wait
// for it to be cleared 
#define APIO_IRQ_SET_WAIT_REL(X)      (0xC030 | ((X) & 0x07))

// Jump unconditionally to label X within this PIO program 
#define APIO_JMP(X)                  (0x0000 | ((X) & 0x1F))

// Jump to label if X register is zero
#define APIO_JMP_NOT_X(DEST)         (0x0020 | ((DEST) & 0x1F))

// Jump to label if X register is non-zero and then decrement Y after the test
#define APIO_JMP_X_DEC(DEST)         (0x0040 | ((DEST) & 0x1F))

// Jump to label if Y register is zero
#define APIO_JMP_NOT_Y(DEST)         (0x0060 | ((DEST) & 0x1F))

// Jump to label if Y register is non-zero and then decrement X after the test
#define APIO_JMP_Y_DEC(DEST)         (0x0080 | ((DEST) & 0x1F))

// Jump to label if X register is not equal to Y register
#define APIO_JMP_X_NOT_Y(DEST)       (0x00A0 | ((DEST) & 0x1F))

// Jump to label if pin specified as the EXECCTRL JMP_PIN is high
#define APIO_JMP_PIN(X)              (0x00C0 | ((X) & 0x1F))

// Jump to label if the OSR is not empty
#define APIO_JMP_NOT_OSRE(X)         (0x00E0 | ((X) & 0x1F))

// MOV to PINS (uses OUT pin mapping)
#define APIO_MOV_PINS_PINS           0xA000
#define APIO_MOV_PINS_X              0xA001
#define APIO_MOV_PINS_Y              0xA002
#define APIO_MOV_PINS_NULL           0xA003
#define APIO_MOV_PINS_STATUS         0xA005
#define APIO_MOV_PINS_ISR            0xA006
#define APIO_MOV_PINS_OSR            0xA007

// MOV to X
#define APIO_MOV_X_PINS              0xA020
#define APIO_MOV_X_X                 0xA021
#define APIO_MOV_X_STATUS            0xA025
#define APIO_MOV_X_Y                 0xA022
#define APIO_MOV_X_NULL              0xA023
#define APIO_MOV_X_ISR               0xA026
#define APIO_MOV_X_OSR               0xA027

// MOV to Y
#define APIO_MOV_Y_PINS              0xA040
#define APIO_MOV_Y_X                 0xA041
#define APIO_MOV_Y_Y                 0xA042
#define APIO_MOV_Y_NULL              0xA043
#define APIO_MOV_Y_STATUS            0xA045
#define APIO_MOV_Y_ISR               0xA046
#define APIO_MOV_Y_OSR               0xA047

// MOV to PINDIRS (uses OUT pin mapping)
#define APIO_MOV_PINDIRS_PINS        0xA060
#define APIO_MOV_PINDIRS_X           0xA061
#define APIO_MOV_PINDIRS_Y           0xA062
#define APIO_MOV_PINDIRS_NULL        0xA063
#define APIO_MOV_PINDIRS_STATUS      0xA065
#define APIO_MOV_PINDIRS_ISR         0xA066
#define APIO_MOV_PINDIRS_OSR         0xA067
#define APIO_MOV_PINDIRS_NOT_NULL    0xA06B // invert

// MOV to EXEC (executes source as instruction next cycle)
#define APIO_MOV_EXEC_PINS           0xA080
#define APIO_MOV_EXEC_X              0xA081
#define APIO_MOV_EXEC_Y              0xA082
#define APIO_MOV_EXEC_NULL           0xA083
#define APIO_MOV_EXEC_STATUS         0xA085
#define APIO_MOV_EXEC_ISR            0xA086
#define APIO_MOV_EXEC_OSR            0xA087

// MOV to PC (unconditional jump)
#define APIO_MOV_PC_PINS             0xA0A0
#define APIO_MOV_PC_X                0xA0A1
#define APIO_MOV_PC_Y                0xA0A2
#define APIO_MOV_PC_NULL             0xA0A3
#define APIO_MOV_PC_STATUS           0xA0A5
#define APIO_MOV_PC_ISR              0xA0A6
#define APIO_MOV_PC_OSR              0xA0A7

// MOV to ISR (resets input shift counter to 0)
#define APIO_MOV_ISR_PINS            0xA0C0
#define APIO_MOV_ISR_X               0xA0C1
#define APIO_MOV_ISR_Y               0xA0C2
#define APIO_MOV_ISR_NULL            0xA0C3
#define APIO_MOV_ISR_STATUS          0xA0C5
#define APIO_MOV_ISR_ISR             0xA0C6
#define APIO_MOV_ISR_OSR             0xA0C7

// MOV to OSR (resets output shift counter to 0)
#define APIO_MOV_OSR_PINS            0xA0E0
#define APIO_MOV_OSR_X               0xA0E1
#define APIO_MOV_OSR_Y               0xA0E2
#define APIO_MOV_OSR_NULL            0xA0E3
#define APIO_MOV_OSR_STATUS          0xA0E5
#define APIO_MOV_OSR_ISR             0xA0E6
#define APIO_MOV_OSR_OSR             0xA0E7

// No operation (move Y to Y)
#define APIO_NOP                     0xA042

// Move data from OSR to the output pins
#define APIO_OUT_PINS(NUM)           (0x6000 | ((NUM) & 0x1F))

// Move data from OSR to X register
#define APIO_OUT_X(NUM)              (0x6020 | ((NUM) & 0x1F))

// Move data from OSR to Y register
#define APIO_OUT_Y(NUM)              (0x6040 | ((NUM) & 0x1F))

// Move data from OSR to null (discard)
#define APIO_OUT_NULL(NUM)           (0x6060 | ((NUM) & 0x1F))

// Move data from OSR to pin directions
#define APIO_OUT_PINDIRS(NUM)        (0x6080 | ((NUM) & 0x1F))

// Move data from OSR to PC
#define APIO_OUT_PC(NUM)             (0x60A0 | ((NUM) & 0x1F))

// Move data from OSR to ISR
#define APIO_OUT_ISR(NUM)            (0x60C0 | ((NUM) & 0x1F))

// Move data from OSR to EXEC (execute as instruction)
#define APIO_OUT_EXEC(NUM)           (0x60E0 | ((NUM) & 0x1F))

// Pull data from the TX FIFO into the OSR, blocking if FIFO is empty
#define APIO_PULL_BLOCK              0x80A0

// Pull from TX FIFO into OSR, non-blocking (copies X to OSR if empty)
#define APIO_PULL_NOBLOCK            0x8080

// Pull from TX FIFO if OSR shift threshold met, blocking
#define APIO_PULL_IFEMPTY_BLOCK      0x80E0

// Pull from TX FIFO if OSR shift threshold met, non-blocking
#define APIO_PULL_IFEMPTY_NOBLOCK    0x80C0

// Push data from the ISR into the RX FIFO, blocking if FIFO is full
#define APIO_PUSH_BLOCK              0x8020

// Push ISR to RX FIFO, non-blocking (continues if full)
#define APIO_PUSH_NOBLOCK            0x8000

// Push ISR to RX FIFO if shift threshold met, blocking
#define APIO_PUSH_IFFULL_BLOCK       0x8060

// Push ISR to RX FIFO if shift threshold met, non-blocking
#define APIO_PUSH_IFFULL_NOBLOCK     0x8040

// Set output pins to VALUE (0-31)
#define APIO_SET_PINS(VALUE)         (0xE000 | ((VALUE) & 0x1F))

// Set X register to VALUE (0-31)
#define APIO_SET_X(VALUE)            (0xE020 | ((VALUE) & 0x1F))

// Set Y register to VALUE (0-31)
#define APIO_SET_Y(VALUE)            (0xE040 | ((VALUE) & 0x1F))

// Set output pin directions to VALUE (0-31)
#define APIO_SET_PIN_DIRS(VALUE)     (0xE080 | ((VALUE) & 0x1F))

// Wait for GPIO to be high
#define APIO_WAIT_GPIO_HIGH(X)       (0x2080 | ((X) & 0x1F))

// Wait for GPIO to be low
#define APIO_WAIT_GPIO_LOW(X)        (0x2000 | ((X) & 0x1F))

// Wait for the specified pin to go high
#define APIO_WAIT_PIN_HIGH(X)        (0x20A0 | ((X) & 0x1F))

// Wait for the specified pin to go low
#define APIO_WAIT_PIN_LOW(X)         (0x2020 | ((X) & 0x1F))

// Wait for JMP_PIN to be high
#define APIO_WAIT_JMP_PIN_HIGH()     (0x20E0)

// Wait for JMP_PIN to be low
#define APIO_WAIT_JMP_PIN_LOW()      (0x2060)

// Wait for one of this PIO block's IRQs to go high.  Clears the IRQ
// after the instruction (so other PIOs waiting at the same time will also be
// triggered).
#define APIO_WAIT_IRQ_HIGH(X)        (0x20C0 | ((X) & 0x07))

// Wait for one of the previous PIO block's IRQs to go high.
#define APIO_WAIT_IRQ_HIGH_PREV(X)   (0x20C8 | ((X) & 0x07))

// Wait for one of the next PIO block's IRQs to go high.
#define APIO_WAIT_IRQ_HIGH_NEXT(X)   (0x20D8 | ((X) & 0x07))

// Wait for an IRQ to go high, using relative addressing mode
#define APIO_WAIT_IRQ_HIGH_REL(X)    (0x20D0| ((X) & 0x07))

// Wait for one of this PIO block's IRQs to go low.
#define APIO_WAIT_IRQ_LOW(X)         (0x2040 | ((X) & 0x07))

// Wait for one of the previous PIO block's IRQs to go low.
#define APIO_WAIT_IRQ_LOW_PREV(X)    (0x2048 | ((X) & 0x07))

// Wait for one of the next PIO block's IRQs to go low.
#define APIO_WAIT_IRQ_LOW_NEXT(X)    (0x2058 | ((X) & 0x07))

// Wait for an IRQ to go low, using relative addressing mode
#define APIO_WAIT_IRQ_LOW_REL(X)     (0x2050| ((X) & 0x07))

#if !defined(APIO_EMULATION)
#define APIO_ASM_WFI()               __asm volatile("wfi")
#else // APIO_EMULATION
#define APIO_ASM_WFI()               return 0
#endif // !APIO_EMULATION

#endif // APIO_H