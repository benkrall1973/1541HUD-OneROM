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
// RP2350 PIO register definitions

#ifndef APIO_REG_H
#define APIO_REG_H

#include <stdint.h>

// Structure representing the registers of a PIO state machine.
//
// These are at 0xc8 + (SM_NUM * 0x18) from the base of the PIO block register
// space.
typedef struct pio_sm_reg {
    uint32_t clkdiv;
    uint32_t execctrl;
    uint32_t shiftctrl;
    uint32_t addr;  // Read only
    uint32_t instr;
    uint32_t pinctrl; 
} pio_sm_reg_t;

// Base register addresses
#define APIO0_BASE              (0x50200000U)
#define APIO1_BASE              (0x50300000U)
#define APIO2_BASE              (0x50400000U)
#define APIO_RESETS_BASE        (0x40020000U)
#define APIO_IO_BANK0_BASE      (0x40028000U)
#define APIO_PADS_BANK0_BASE    (0x40038000U)

// Registers used for configuring GPIOs
#define APIO_RESET_RESET            (*((volatile uint32_t *)(APIO_RESETS_BASE + 0x00)))
#define APIO_RESET_DONE             (*((volatile uint32_t *)(APIO_RESETS_BASE + 0x08)))
#define APIO_RESET_IOBANK0          (1 << 6)
#define APIO_RESET_JTAG             (1 << 8)
#define APIO_RESET_PADS_BANK0       (1 << 9)
#define APIO_RESET_PIO0             (1 << 11)
#define APIO_RESET_PIO1             (1 << 12)
#define APIO_RESET_PIO2             (1 << 13)

// GPIO function select values (FUNCSEL field of GPIO_CTRL)
#define APIO_GPIO_CTRL_FUNC_SIO     0x05
#define APIO_GPIO_CTRL_FUNC_PIO0    0x06
#define APIO_GPIO_CTRL_FUNC_PIO1    0x07
#define APIO_GPIO_CTRL_FUNC_PIO2    0x08

#define APIO_GPIO_CTRL_OFFSET       0x004
#define APIO_GPIO_SPACING           0x008
#define APIO_GPIO_CTRL(pin)         (*(volatile uint32_t*)(APIO_IO_BANK0_BASE + APIO_GPIO_CTRL_OFFSET + (pin)*APIO_GPIO_SPACING))

// Pad register bits
#define APIO_PAD_OFFSET_START       0x004
#define APIO_PAD_SPACING            0x004
#define APIO_GPIO_PAD(pin)          (*(volatile uint32_t*)(APIO_PADS_BANK0_BASE + APIO_PAD_OFFSET_START + pin*APIO_PAD_SPACING))

#define APIO_PAD_ISO_BIT            (1 << 8)    // Pad isolation
#define APIO_PAD_OUTPUT_DIS_BIT     (1 << 7)    // Output disable (OD)
#define APIO_PAD_INPUT_EN_BIT       (1 << 6)    // Input enable (IE)
#define APIO_PAD_DRIVE_OFFSET       4
#define APIO_PAD_DRIVE_MASK         (0x3 << APIO_PAD_DRIVE_OFFSET)
#define APIO_PAD_DRIVE(x)           (((x) & 0x3) << APIO_PAD_DRIVE_OFFSET)
#define APIO_PAD_PUE_BIT            (1 << 3)    // Pull-up enable
#define APIO_PAD_PDE_BIT            (1 << 2)    // Pull-down enable
#define APIO_PAD_SLEWFAST_BIT       (1 << 0)    // Slew rate fast

// Drive strength values (DRIVE field of pad register, bits 5:4)
// Hardware reset default is APIO_DRIVE_4MA.
#define APIO_DRIVE_2MA              0x0
#define APIO_DRIVE_4MA              0x1
#define APIO_DRIVE_8MA              0x2
#define APIO_DRIVE_12MA             0x3

// GPIO CTRL INOVER field
#define APIO_GPIO_CTRL_INOVER_INVERT (0b01 << 16)
#define APIO_GPIO_CTRL_INOVER_LOW    (0b10 << 16)
#define APIO_GPIO_CTRL_INOVER_HIGH   (0b11 << 16)
#define APIO_GPIO_CTRL_INOVER_MASK   (0b11 << 16)

// PIO register offsets
#define APIO_CTRL_OFFSET                (0x00)
#define APIO_FSTAT_OFFSET               (0x04)
#define APIO_FDEBUG_OFFSET              (0x08)
#define APIO_FLEVEL_OFFSET              (0x0C)
#define APIO_TXF_OFFSET                 (0x10)
#define APIO_RXF_OFFSET                 (0x20)
#define APIO_IRQ_OFFSET                 (0x30)
#define APIO_IRQ_FORCE_OFFSET           (0x34)
#define APIO_INPUT_SYNC_BYPASS_OFFSET   (0x38)
#define APIO_INSTR_MEM_OFFSET           (0x48)
#define APIO_SM_REG_OFFSET              (0xC8)
#define APIO_SM_RXF_OFFSET              (0x128)
#define APIO_SM_TXF_OFFSET              (0x138)
#define APIO_GPIOBASE_OFFSET            (0x168)

/// Macros for accessing PIO control registers
#define APIO0_CTRL          (*(volatile uint32_t *)(APIO0_BASE + APIO_CTRL_OFFSET))
#define APIO1_CTRL          (*(volatile uint32_t *)(APIO1_BASE + APIO_CTRL_OFFSET))
#define APIO2_CTRL          (*(volatile uint32_t *)(APIO2_BASE + APIO_CTRL_OFFSET))
#define APIO0_FSTAT         (*(volatile uint32_t *)(APIO0_BASE + APIO_FSTAT_OFFSET))
#define APIO1_FSTAT         (*(volatile uint32_t *)(APIO1_BASE + APIO_FSTAT_OFFSET))
#define APIO2_FSTAT         (*(volatile uint32_t *)(APIO2_BASE + APIO_FSTAT_OFFSET))
#define APIO0_FLEVEL        (*(volatile uint32_t *)(APIO0_BASE + APIO_FLEVEL_OFFSET))
#define APIO1_FLEVEL        (*(volatile uint32_t *)(APIO1_BASE + APIO_FLEVEL_OFFSET))
#define APIO2_FLEVEL        (*(volatile uint32_t *)(APIO2_BASE + APIO_FLEVEL_OFFSET))
#define APIO0_SM_TXF(X)     (*(volatile uint32_t *)(APIO0_BASE + APIO_TXF_OFFSET + ((X) * 0x04)))
#define APIO1_SM_TXF(X)     (*(volatile uint32_t *)(APIO1_BASE + APIO_TXF_OFFSET + ((X) * 0x04)))
#define APIO2_SM_TXF(X)     (*(volatile uint32_t *)(APIO2_BASE + APIO_TXF_OFFSET + ((X) * 0x04)))
#define APIO0_SM_RXF(X)     (*(volatile uint32_t *)(APIO0_BASE + APIO_RXF_OFFSET + ((X) * 0x04)))
#define APIO1_SM_RXF(X)     (*(volatile uint32_t *)(APIO1_BASE + APIO_RXF_OFFSET + ((X) * 0x04)))
#define APIO2_SM_RXF(X)     (*(volatile uint32_t *)(APIO2_BASE + APIO_RXF_OFFSET + ((X) * 0x04)))
#define APIO0_IRQ           (*(volatile uint32_t *)(APIO0_BASE + APIO_IRQ_OFFSET))
#define APIO1_IRQ           (*(volatile uint32_t *)(APIO1_BASE + APIO_IRQ_OFFSET))
#define APIO2_IRQ           (*(volatile uint32_t *)(APIO2_BASE + APIO_IRQ_OFFSET))
#define APIO0_IRQ_FORCE     (*(volatile uint32_t *)(APIO0_BASE + APIO_IRQ_FORCE_OFFSET))
#define APIO1_IRQ_FORCE     (*(volatile uint32_t *)(APIO1_BASE + APIO_IRQ_FORCE_OFFSET))
#define APIO2_IRQ_FORCE     (*(volatile uint32_t *)(APIO2_BASE + APIO_IRQ_FORCE_OFFSET))
#define APIO0_INPUT_SYNC_BYPASS  (*(volatile uint32_t *)(APIO0_BASE + APIO_INPUT_SYNC_BYPASS_OFFSET))
#define APIO1_INPUT_SYNC_BYPASS  (*(volatile uint32_t *)(APIO1_BASE + APIO_INPUT_SYNC_BYPASS_OFFSET))
#define APIO2_INPUT_SYNC_BYPASS  (*(volatile uint32_t *)(APIO2_BASE + APIO_INPUT_SYNC_BYPASS_OFFSET))
#define APIO0_GPIOBASE (*(volatile uint32_t *)(APIO0_BASE + APIO_GPIOBASE_OFFSET))
#define APIO1_GPIOBASE (*(volatile uint32_t *)(APIO1_BASE + APIO_GPIOBASE_OFFSET))
#define APIO2_GPIOBASE (*(volatile uint32_t *)(APIO2_BASE + APIO_GPIOBASE_OFFSET))

// GPIOBASE
#define APIO_GPIOBASE_VAL_0     (0)
#define APIO_GPIOBASE_VAL_16    (1 << 4)

// Macros for PIO control registers
#define APIO_CTRL_SM_ENABLE(X)      ((X & 0xf) << 0)
#define APIO0_CTRL_SM_ENABLE(X)     APIO0_CTRL = APIO_CTRL_SM_ENABLE(X)
#define APIO1_CTRL_SM_ENABLE(X)     APIO1_CTRL = APIO_CTRL_SM_ENABLE(X)
#define APIO2_CTRL_SM_ENABLE(X)     APIO2_CTRL = APIO_CTRL_SM_ENABLE(X)

// Macros for PIO FSTAT registers
#define APIO_FSTAT_SMX_RX_EMPTY_BIT(X)       (1 << (X + 8))
#define APIO0_FSTAT_SMX_RX_EMPTY(X)          (APIO_FSTAT_SMX_RX_EMPTY_BIT(X) & APIO0_FSTAT)

// Macros for filling PIO instruction memory
#define APIO0_INSTR_MEM(X)       (*(volatile uint32_t *)(APIO0_BASE + APIO_INSTR_MEM_OFFSET + ((X) * 4)))
#define APIO1_INSTR_MEM(X)       (*(volatile uint32_t *)(APIO1_BASE + APIO_INSTR_MEM_OFFSET + ((X) * 4)))
#define APIO2_INSTR_MEM(X)       (*(volatile uint32_t *)(APIO2_BASE + APIO_INSTR_MEM_OFFSET + ((X) * 4)))

// Macros for accessing PIO state machine registers
#define APIO0_SM_REG(X)      ((volatile pio_sm_reg_t *)((uintptr_t)APIO0_BASE + APIO_SM_REG_OFFSET + ((X) * 0x18)))
#define APIO1_SM_REG(X)      ((volatile pio_sm_reg_t *)((uintptr_t)APIO1_BASE + APIO_SM_REG_OFFSET + ((X) * 0x18)))
#define APIO2_SM_REG(X)      ((volatile pio_sm_reg_t *)((uintptr_t)APIO2_BASE + APIO_SM_REG_OFFSET + ((X) * 0x18)))

// Macros to build PIO SM registers

// CLKDIV
#define APIO_CLKDIV(INT, FRAC)           (((INT) & 0xFFFF) << 16 | ((FRAC) & 0xFF) << 8)
#define APIO_CLKDIV_INT_FROM_REG(REG)    (((REG) >> 16) & 0xFFFF)
#define APIO_CLKDIV_FRAC_FROM_REG(REG)   (((REG) >> 8) & 0xFF)

// EXECCTRL
#define APIO_WRAP_BOTTOM_AS_REG(X)  (((X) & 0x1F) << 7)
#define APIO_WRAP_TOP_AS_REG(X)     (((X) & 0x1F) << 12)
#define APIO_EXECCTRL_JMP_PIN(X)        (((X) & 0x1F) << 24)
#define APIO_WRAP_TOP_FROM_REG(REG)     (((REG) >> 12) & 0x1F)
#define APIO_WRAP_BOTTOM_FROM_REG(REG)  (((REG) >> 7) & 0x1F)

// EXECCTRL STATUS_SEL (bits 6:5)
#define APIO_STATUS_SEL_TXLEVEL     (0x0 << 5)
#define APIO_STATUS_SEL_RXLEVEL     (0x1 << 5)
#define APIO_STATUS_SEL_IRQ         (0x2 << 5)

// EXECCTRL STATUS_N (bits 4:0)
#define APIO_STATUS_N(n)            ((n) & 0x1F)

// STATUS_N named values for IRQ mode
#define APIO_STATUS_N_IRQ           0x00
#define APIO_STATUS_N_IRQ_PREVPIO   0x08
#define APIO_STATUS_N_IRQ_NEXTPIO   0x10

// SHIFTCTRL
#define APIO_IN_COUNT(X)         (((X) & 0x1F) << 0)
#define APIO_AUTOPUSH            (1 << 16)
#define APIO_AUTOPULL            (1 << 17)
#define APIO_IN_SHIFTDIR_R       (1 << 18)
#define APIO_IN_SHIFTDIR_L       (0 << 18)
#define APIO_OUT_SHIFTDIR_R      (1 << 19)
#define APIO_OUT_SHIFTDIR_L      (0 << 19)
#define APIO_PUSH_THRESH(X)      (((X) & 0x1F) << 20)
#define APIO_PULL_THRESH(X)      (((X) & 0x1F) << 25)

// PINCTRL
#define APIO_OUT_BASE(X)         (((X) & 0x1F) << 0)
#define APIO_SET_BASE(X)         (((X) & 0x1F) << 5)
#define APIO_SIDE_SET_BASE(X)    (((X) & 0x1F) << 10)
#define APIO_IN_BASE(X)          (((X) & 0x1F) << 15)
#define APIO_OUT_COUNT(X)        (((X) & 0x3F) << 20)
#define APIO_SET_COUNT(X)        (((X) & 0x07) << 26)
#define APIO_SIDE_SET_COUNT(X)   (((X) & 0x07) << 29)

// EXECCTRL
#define APIO_EXECCTRL_EXEC_STALLED_FROM_REG(REG)  (((REG) >> 31) & 0x1u)
#define APIO_EXECCTRL_SIDE_EN_FROM_REG(REG)       (((REG) >> 30) & 0x1u)
#define APIO_EXECCTRL_SIDE_PINDIR_FROM_REG(REG)   (((REG) >> 29) & 0x1u)
#define APIO_EXECCTRL_JMP_PIN_FROM_REG(REG)       (((REG) >> 24) & 0x1Fu)
#define APIO_EXECCTRL_OUT_EN_SEL_FROM_REG(REG)    (((REG) >> 19) & 0x1Fu)
#define APIO_EXECCTRL_INLINE_OUT_EN_FROM_REG(REG) (((REG) >> 18) & 0x1u)
#define APIO_EXECCTRL_OUT_STICKY_FROM_REG(REG)    (((REG) >> 17) & 0x1u)
#define APIO_EXECCTRL_STATUS_SEL_FROM_REG(REG)    (((REG) >>  5) & 0x3u)
#define APIO_EXECCTRL_STATUS_N_FROM_REG(REG)      ((REG) & 0x1Fu)
 
// SHIFTCTRL
#define APIO_SHIFTCTRL_FJOIN_RX_FROM_REG(REG)     (((REG) >> 31) & 0x1u)
#define APIO_SHIFTCTRL_FJOIN_TX_FROM_REG(REG)     (((REG) >> 30) & 0x1u)
#define APIO_PULL_THRESH_FROM_REG(REG)            (((REG) >> 25) & 0x1Fu)
#define APIO_PUSH_THRESH_FROM_REG(REG)            (((REG) >> 20) & 0x1Fu)
#define APIO_SHIFTCTRL_FJOIN_RX_PUT_FROM_REG(REG) (((REG) >> 15) & 0x1u)
#define APIO_SHIFTCTRL_FJOIN_RX_GET_FROM_REG(REG) (((REG) >> 14) & 0x1u)
#define APIO_IN_COUNT_FROM_REG(REG)               ((REG) & 0x1Fu)
 
// PINCTRL
#define APIO_SIDE_SET_COUNT_FROM_REG(REG)  (((REG) >> 29) & 0x7u)
#define APIO_SET_COUNT_FROM_REG(REG)       (((REG) >> 26) & 0x7u)
#define APIO_OUT_COUNT_FROM_REG(REG)       (((REG) >> 20) & 0x3Fu)
#define APIO_IN_BASE_FROM_REG(REG)         (((REG) >> 15) & 0x1Fu)
#define APIO_SIDE_SET_BASE_FROM_REG(REG)   (((REG) >> 10) & 0x1Fu)
#define APIO_SET_BASE_FROM_REG(REG)        (((REG) >>  5) & 0x1Fu)
#define APIO_OUT_BASE_FROM_REG(REG)        ((REG) & 0x1Fu)
 
// PUSH_THRESH, PULL_THRESH and IN_COUNT encode 32 as 0
#define APIO_THRESH32(x) ((x) ? (uint8_t)(x) : (uint8_t)32)

// Macro to access a PIO state machine RX FIFO entry
#define APIO0_SM_X_RXF_Y(X, Y)   (*(volatile uint32_t *)(APIO0_BASE + APIO_SM_RXF_OFFSET + ((X) * 0x10) + ((Y) * 4)))
#define APIO1_SM_X_RXF_Y(X, Y)   (*(volatile uint32_t *)(APIO1_BASE + APIO_SM_RXF_OFFSET + ((X) * 0x10) + ((Y) * 4)))
#define APIO2_SM_X_RXF_Y(X, Y)   (*(volatile uint32_t *)(APIO2_BASE + APIO_SM_RXF_OFFSET + ((X) * 0x10) + ((Y) * 4)))

// Macros to construct DREQ values
#define APIO_DREQ_PIO_X_SM_Y_TX(X, Y)      (0 + (X * 8) + Y)
#define APIO_DREQ_PIO_X_SM_Y_RX(X, Y)      (4 + (X * 8) + Y)

#endif // APIO_REG_H