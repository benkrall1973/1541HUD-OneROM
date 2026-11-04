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
// Disassembly routines

#ifndef APIO_DIS_H
#define APIO_DIS_H

#include <stdint.h>

// Logging is disabled by default. To enable, define APIO_LOG_ENABLE as the
// logging function to use before including this header, or pass it as a
// compiler flag, e.g.:
//
//   -DAPIO_LOG_ENABLE=printf
//
// When enabled, APIO_LOG(...) and APIO_LOG_SM(NAME) will call the provided
// function. When disabled, both expand to no-ops and the disassembler and
// logging functions are excluded from the build entirely.
//
// You must also have #define APIO_LOG_IMPL 1 in one C file to include the
// implementation of the logging functions.
#if defined(APIO_LOG_ENABLE)

#define APIO_LOG(...) APIO_LOG_ENABLE(__VA_ARGS__)

void apio_instruction_decoder(uint16_t instr, char *out_str, uint8_t start_offset);
void apio_log_sm(
    const char *sm_name,
    uint8_t pio_block,
    uint8_t pio_sm,
    uint16_t *instr_scratch,
    uint8_t first_instr,
    uint8_t start,
    uint8_t end
);

#if !defined(APIO_EMULATION)
#define APIO_LOG_SM(NAME)                       \
    apio_log_sm(                                \
        NAME,                                   \
        __blk,                                  \
        __sm,                                   \
        instr_scratch,                          \
        __pio_first_instr[__blk][__sm],         \
        __pio_start[__blk][__sm],               \
        __pio_end[__blk][__sm]                  \
    )
#else // APIO_EMULATION
#define APIO_LOG_SM(NAME)                                        \
    apio_log_sm(                                                 \
        NAME,                                                    \
        __blk,                                                   \
        __sm,                                                    \
        _apio_emulated_pio.instr[__blk],                         \
        _apio_emulated_pio.first_instr[__blk][__sm],             \
        __pio_start[__blk][__sm],                                \
        __pio_end[__blk][__sm]                                   \
    )
#endif // !APIO_EMULATION

#else // !APIO_LOG_ENABLE

#define APIO_LOG(...)        do {} while(0)
#define APIO_LOG_SM(NAME)    do {} while(0)

#endif // APIO_LOG_ENABLE

void apio_instruction_decoder(uint16_t instr, char *out_str, uint8_t start_offset);
void apio_log_sm(
    const char *sm_name,
    uint8_t pio_block,
    uint8_t pio_sm,
    uint16_t *instr_scratch,
    uint8_t first_instr,
    uint8_t start,
    uint8_t end
);

#if defined(APIO_LOG_ENABLE) && defined(APIO_LOG_IMPL)

static const char* piorom_get_jmp_condition(uint8_t cond) {
    switch (cond) {
        case 0b000: return "";  // LCOV_EXCL_LINE
        case 0b001: return "!x";
        case 0b010: return "x--";
        case 0b011: return "!y";
        case 0b100: return "y--";
        case 0b101: return "x!=y";
        case 0b110: return "pin";
        case 0b111: return "!osre";
        default: return "???";  // LCOV_EXCL_LINE
    }
}

static const char* piorom_get_wait_source(uint8_t src) {
    switch (src) {
        case 0b00: return "gpio";
        case 0b01: return "pin";
        case 0b10: return "irq";
        case 0b11: return "jmppin";
        default: return "???";  // LCOV_EXCL_LINE
    }
}

static const char* piorom_get_in_source(uint8_t src) {
    switch (src) {
        case 0b000: return "pins";
        case 0b001: return "x";
        case 0b010: return "y";
        case 0b011: return "null";
        case 0b100: return "reserved";  // LCOV_EXCL_LINE
        case 0b101: return "reserved";  // LCOV_EXCL_LINE
        case 0b110: return "isr";
        case 0b111: return "osr";
        default: return "???";  // LCOV_EXCL_LINE
    }
}

static const char* piorom_get_out_dest(uint8_t dest) {
    switch (dest) {
        case 0b000: return "pins";
        case 0b001: return "x";
        case 0b010: return "y";
        case 0b011: return "null";
        case 0b100: return "pindirs";
        case 0b101: return "pc";
        case 0b110: return "isr";
        case 0b111: return "exec";
        default: return "???";  // LCOV_EXCL_LINE
    }
}

static const char* piorom_get_mov_dest(uint8_t dest) {
    switch (dest) {
        case 0b000: return "pins";
        case 0b001: return "x";
        case 0b010: return "y";
        case 0b011: return "pindirs";
        case 0b100: return "exec";
        case 0b101: return "pc";
        case 0b110: return "isr";
        case 0b111: return "osr";
        default: return "???";  // LCOV_EXCL_LINE
    }
}

static const char* piorom_get_mov_op(uint8_t op) {
    switch (op) {
        case 0b00: return "";
        case 0b01: return "~";
        case 0b10: return "::";
        case 0b11: return "reserved";  // LCOV_EXCL_LINE
        default: return "???";  // LCOV_EXCL_LINE
    }
}

static const char* piorom_get_mov_source(uint8_t src) {
    switch (src) {
        case 0b000: return "pins";
        case 0b001: return "x";
        case 0b010: return "y";
        case 0b011: return "null";
        case 0b100: return "reserved";  // LCOV_EXCL_LINE
        case 0b101: return "status";
        case 0b110: return "isr";
        case 0b111: return "osr";
        default: return "???";  // LCOV_EXCL_LINE
    }
}

static const char* piorom_get_set_dest(uint8_t dest) {
    switch (dest) {
        case 0b000: return "pins";
        case 0b001: return "x";
        case 0b010: return "y";
        case 0b011: return "reserved";  // LCOV_EXCL_LINE
        case 0b100: return "pindirs";
        case 0b101: return "reserved";  // LCOV_EXCL_LINE
        case 0b110: return "reserved";  // LCOV_EXCL_LINE
        case 0b111: return "reserved";  // LCOV_EXCL_LINE
        default: return "???";  // LCOV_EXCL_LINE
    }
}

// Returns the name for an EXECCTRL STATUS_SEL value.
// sel is APIO_EXECCTRL_STATUS_SEL_FROM_REG(execctrl), i.e. (reg >> 5) & 0x3,
// matching APIO_STATUS_SEL_{TXLEVEL,RXLEVEL,IRQ} >> 5.
static __attribute__((unused)) const char* apio_get_status_sel_str(uint8_t sel) {
    switch (sel) {
        case 0: return "TXLEVEL";  // APIO_STATUS_SEL_TXLEVEL >> 5
        case 1: return "RXLEVEL";  // APIO_STATUS_SEL_RXLEVEL >> 5
        case 2: return "IRQ";      // APIO_STATUS_SEL_IRQ     >> 5
        default: return "???";
    }
}

static char* append_str(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    return dest;
}

static char* append_char(char* dest, char c) {
    *dest++ = c;
    return dest;
}

static char* append_uint(char* dest, uint32_t val) {
    if (val == 0) {
        *dest++ = '0';
        return dest;
    }
    
    char temp[11];
    int i = 0;
    while (val > 0) {
        temp[i++] = '0' + (val % 10);
        val /= 10;
    }
    
    while (i > 0) {
        *dest++ = temp[--i];
    }
    return dest;
}

static char* append_delay(char* dest, uint8_t delay) {
    if (delay > 0) {
        dest = append_str(dest, " [");
        dest = append_uint(dest, delay);
        dest = append_char(dest, ']');
    }
    return dest;
}

// Decodes a single PIO instruction into a human-readable string
// Arguments:
// - instr: The 16-bit PIO instruction to decode
// - out_str: Buffer to write the decoded instruction string (provide at least
//   64 bytes)
// - start_offset: The instruction index of the first instruction in the
//   program.  Used for relative JMP addresses.  Set at 0 for absolute (PIO
//   block-wide addresses.
void apio_instruction_decoder(uint16_t instr, char *out_str, uint8_t start_offset) {
    uint8_t opcode = (instr >> 13) & 0x7;
    uint8_t delay = (instr >> 8) & 0x1F;
    char* p;
    
    switch (opcode) {
        case 0b000: { // JMP
            uint8_t condition = (instr >> 5) & 0x7;
            uint8_t address = instr & 0x1F;
            p = out_str;
            p = append_str(p, "jmp ");
            if (condition != 0) {
                p = append_str(p, piorom_get_jmp_condition(condition));
                p = append_str(p, ", ");
            }
            p = append_uint(p, address - start_offset);
            p = append_delay(p, delay);
            *p = '\0';
            break;
        }
        
        case 0b001: { // WAIT
            uint8_t pol = (instr >> 7) & 0x1;
            uint8_t source = (instr >> 5) & 0x3;
            uint8_t idx_mode = (instr >> 3) & 0x3;
            p = out_str;
            p = append_str(p, "wait ");
            p = append_uint(p, pol);
            p = append_char(p, ' ');
            p = append_str(p, piorom_get_wait_source(source));

            uint8_t index;
            if (source == 0b10) {
                // IRQ
                index = instr & 0x7;

                // prev/next
                if (idx_mode == 0b01) {
                    p = append_str(p, " prev");
                } else if (idx_mode == 0b11) {
                    p = append_str(p, " next");
                }
            } else {
                // Other
                index = instr & 0x1F;
            }


            p = append_str(p, " ");
            p = append_uint(p, index);
            p = append_delay(p, delay);
            *p = '\0';
            break;
        }
        
        case 0b010: { // IN
            uint8_t source = (instr >> 5) & 0x7;
            uint8_t bitcount = instr & 0x1F;
            p = out_str;
            p = append_str(p, "in ");
            p = append_str(p, piorom_get_in_source(source));
            p = append_str(p, ", ");
            p = append_uint(p, bitcount);
            p = append_delay(p, delay);
            *p = '\0';
            break;
        }
        
        case 0b011: { // OUT
            uint8_t dest = (instr >> 5) & 0x7;
            uint8_t bitcount = instr & 0x1F;
            p = out_str;
            p = append_str(p, "out ");
            p = append_str(p, piorom_get_out_dest(dest));
            p = append_str(p, ", ");
            p = append_uint(p, bitcount);
            p = append_delay(p, delay);
            *p = '\0';
            break;
        }
        
        case 0b100: { // PUSH/PULL/MOV indexed
            uint8_t bit7 = (instr >> 7) & 0x1;
            uint8_t bit4 = (instr >> 4) & 0x1;
            p = out_str;
            
            if (bit4 == 0) {
                // PUSH or PULL
                uint8_t if_flag = (instr >> 6) & 0x1;
                uint8_t block = (instr >> 5) & 0x1;
                
                if (bit7 == 0) {
                    // PUSH
                    p = append_str(p, "push");
                    if (if_flag) {
                        p = append_str(p, " iffull ");
                    } else {
                        p = append_char(p, ' ');
                    }
                    p = append_str(p, block ? "block" : "noblock");
                } else {
                    // PULL
                    p = append_str(p, "pull");
                    if (if_flag) {
                        p = append_str(p, " ifempty ");
                    } else {
                        p = append_char(p, ' ');
                    }
                    p = append_str(p, block ? "block" : "noblock");
                }
            } else {
                // MOV indexed
                uint8_t idx_i = (instr >> 3) & 0x1;
                uint8_t index = instr & 0x3;
                
                if (bit7 == 0) {
                    // MOV RX
                    p = append_str(p, "mov rxfifo[");
                    if (idx_i) {
                        p = append_uint(p, index);
                    } else {
                        p = append_char(p, 'y');
                    }
                    p = append_str(p, "], isr");
                } else {
                    // MOV TX
                    p = append_str(p, "mov txfifo[");
                    if (idx_i) {
                        p = append_uint(p, index);
                    } else {
                        p = append_char(p, 'y');
                    }
                    p = append_str(p, "], osr");
                }
            }
            p = append_delay(p, delay);
            *p = '\0';
            break;
        }
        
        case 0b101: { // MOV
            uint8_t dest = (instr >> 5) & 0x7;
            uint8_t op = (instr >> 3) & 0x3;
            uint8_t source = instr & 0x7;
            p = out_str;
            
            // Check for nop (mov y, y)
            if (dest == 0b010 && op == 0b00 && source == 0b010) {
                p = append_str(p, "nop");
            } else {
                p = append_str(p, "mov ");
                p = append_str(p, piorom_get_mov_dest(dest));
                p = append_str(p, ", ");
                p = append_str(p, piorom_get_mov_op(op));
                p = append_str(p, piorom_get_mov_source(source));
            }
            p = append_delay(p, delay);
            *p = '\0';
            break;
        }
        
        case 0b110: { // IRQ
            uint8_t clr = (instr >> 6) & 0x1;
            uint8_t wait = (instr >> 5) & 0x1;
            uint8_t idx_mode = (instr >> 3) & 0x3;
            uint8_t index = instr & 0x7;
            p = out_str;
            p = append_str(p, "irq ");
            
            // prev/next
            if (idx_mode == 0b01) {
                p = append_str(p, "prev ");
            } else if (idx_mode == 0b11) {
                p = append_str(p, "next ");
            }
            
            // set/wait/clear
            if (clr) {
                p = append_str(p, "clear ");
            } else if (wait) {
                p = append_str(p, "wait ");
            }
            
            p = append_uint(p, index);
            
            // rel
            if (idx_mode == 0b10) {
                p = append_str(p, " rel");
            }
            
            p = append_delay(p, delay);
            *p = '\0';
            break;
        }
        
        case 0b111: { // SET
            uint8_t dest = (instr >> 5) & 0x7;
            uint8_t data = instr & 0x1F;
            p = out_str;
            p = append_str(p, "set ");
            p = append_str(p, piorom_get_set_dest(dest));
            p = append_str(p, ", ");
            p = append_uint(p, data);
            p = append_delay(p, delay);
            *p = '\0';
            break;
        }
    }
}

// Log the PIO state machine configuration and program instructions
// Arguments:
// - sm_name: Human readable name of the state machine/program
// - pio_block: PIO block number (0, 1, or 2)
// - pio_sm: PIO state machine number (0-3)
// - instr_scratch: Pointer to the full array of instructions for this block
// - first_instr: Index of the first instruction of this program in instr_scratch
// - start: Index of the .start instruction (where the SM starts execution)
// - end: Index of the last instruction of this program
void apio_log_sm(
    const char *sm_name,
    uint8_t pio_block,
    uint8_t pio_sm,
    uint16_t *instr_scratch,
    uint8_t first_instr,
    uint8_t start,
    uint8_t end
) {
    volatile pio_sm_reg_t *sm_reg;
    char instr[64];
    (void)sm_name;
    
    // Get the correct PIO block's SM register
    if (pio_block == 0) {
        sm_reg = APIO0_SM_REG(pio_sm);
    } else if (pio_block == 1) {
        sm_reg = APIO1_SM_REG(pio_sm);
    } else {
        sm_reg = APIO2_SM_REG(pio_sm);
    }

    uint16_t clkdiv_int  = APIO_CLKDIV_INT_FROM_REG(sm_reg->clkdiv);
    uint8_t  clkdiv_frac = APIO_CLKDIV_FRAC_FROM_REG(sm_reg->clkdiv);
    uint8_t  wrap_bottom = APIO_WRAP_BOTTOM_FROM_REG(sm_reg->execctrl);
    uint8_t  wrap_top    = APIO_WRAP_TOP_FROM_REG(sm_reg->execctrl);

    uint32_t execctrl  = sm_reg->execctrl;
    uint32_t shiftctrl = sm_reg->shiftctrl;
    uint32_t pinctrl   = sm_reg->pinctrl;

    // EXECCTRL fields
    uint8_t exec_stalled  = (uint8_t)APIO_EXECCTRL_EXEC_STALLED_FROM_REG(execctrl);
    uint8_t side_en       = (uint8_t)APIO_EXECCTRL_SIDE_EN_FROM_REG(execctrl);
    uint8_t side_pindir   = (uint8_t)APIO_EXECCTRL_SIDE_PINDIR_FROM_REG(execctrl);
    uint8_t jmp_pin       = (uint8_t)APIO_EXECCTRL_JMP_PIN_FROM_REG(execctrl);
    uint8_t out_en_sel    = (uint8_t)APIO_EXECCTRL_OUT_EN_SEL_FROM_REG(execctrl);
    uint8_t inline_out_en = (uint8_t)APIO_EXECCTRL_INLINE_OUT_EN_FROM_REG(execctrl);
    uint8_t out_sticky    = (uint8_t)APIO_EXECCTRL_OUT_STICKY_FROM_REG(execctrl);
    uint8_t status_sel    = (uint8_t)APIO_EXECCTRL_STATUS_SEL_FROM_REG(execctrl);
    uint8_t status_n      = (uint8_t)APIO_EXECCTRL_STATUS_N_FROM_REG(execctrl);

    // SHIFTCTRL fields
    // Single-bit fields use the existing mask constants directly
    uint8_t fjoin_rx     = (uint8_t)APIO_SHIFTCTRL_FJOIN_RX_FROM_REG(shiftctrl);
    uint8_t fjoin_tx     = (uint8_t)APIO_SHIFTCTRL_FJOIN_TX_FROM_REG(shiftctrl);
    uint8_t fjoin_rx_put = (uint8_t)APIO_SHIFTCTRL_FJOIN_RX_PUT_FROM_REG(shiftctrl);
    uint8_t fjoin_rx_get = (uint8_t)APIO_SHIFTCTRL_FJOIN_RX_GET_FROM_REG(shiftctrl);
    uint8_t autopush     = (shiftctrl & APIO_AUTOPUSH)      ? 1u : 0u;
    uint8_t autopull     = (shiftctrl & APIO_AUTOPULL)      ? 1u : 0u;
    uint8_t in_shiftdir  = (shiftctrl & APIO_IN_SHIFTDIR_R) ? 1u : 0u;
    uint8_t out_shiftdir = (shiftctrl & APIO_OUT_SHIFTDIR_R) ? 1u : 0u;
    uint8_t push_thresh  = APIO_THRESH32(APIO_PUSH_THRESH_FROM_REG(shiftctrl));
    uint8_t pull_thresh  = APIO_THRESH32(APIO_PULL_THRESH_FROM_REG(shiftctrl));
    uint8_t in_count     = APIO_THRESH32(APIO_IN_COUNT_FROM_REG(shiftctrl));

    // PINCTRL fields
    uint8_t out_base      = (uint8_t)APIO_OUT_BASE_FROM_REG(pinctrl);
    uint8_t out_count     = (uint8_t)APIO_OUT_COUNT_FROM_REG(pinctrl);
    uint8_t set_base      = (uint8_t)APIO_SET_BASE_FROM_REG(pinctrl);
    uint8_t set_count     = (uint8_t)APIO_SET_COUNT_FROM_REG(pinctrl);
    uint8_t in_base       = (uint8_t)APIO_IN_BASE_FROM_REG(pinctrl);
    uint8_t sideset_base  = (uint8_t)APIO_SIDE_SET_BASE_FROM_REG(pinctrl);
    uint8_t sideset_count = (uint8_t)APIO_SIDE_SET_COUNT_FROM_REG(pinctrl);

    /* Suppress unused-variable warnings when APIO_LOG_ENABLE is a no-op */
    (void)clkdiv_int;
    (void)clkdiv_frac;
    (void)exec_stalled;
    (void)side_en;
    (void)side_pindir;
    (void)jmp_pin;
    (void)out_en_sel;
    (void)inline_out_en;
    (void)out_sticky;
    (void)status_sel;
    (void)status_n;
    (void)fjoin_rx;
    (void)fjoin_tx;
    (void)fjoin_rx_put;
    (void)fjoin_rx_get;
    (void)autopush;
    (void)autopull;
    (void)in_shiftdir;
    (void)out_shiftdir;
    (void)push_thresh;
    (void)pull_thresh;
    (void)in_count;
    (void)out_base;
    (void)out_count;
    (void)set_base;
    (void)set_count;
    (void)in_base;
    (void)sideset_base;
    (void)sideset_count;

    APIO_LOG("PIO%d:%d %s (%d instructions)", pio_block, pio_sm, sm_name, (end - first_instr + 1));

    APIO_LOG("  CLKDIV: %d.%02d", clkdiv_int, clkdiv_frac);

    APIO_LOG("  EXECCTRL: 0x%08lX%s", (unsigned long)execctrl,
             exec_stalled ? " [STALLED]" : "");
    APIO_LOG("    wrap=%d:%d  side: en=%d pindir=%d  jmp_pin=%d",
        wrap_bottom, wrap_top, side_en, side_pindir, jmp_pin);
    APIO_LOG("    out: sticky=%d inline_en=%d en_sel=%d  status: %s<%d",
        out_sticky, inline_out_en, out_en_sel,
        apio_get_status_sel_str(status_sel), status_n);

    APIO_LOG("  SHIFTCTRL: 0x%08lX", (unsigned long)shiftctrl);
    APIO_LOG("    in:  autopush=%d thresh=%d dir=%c  out: autopull=%d thresh=%d dir=%c",
        autopush, push_thresh, in_shiftdir  ? 'R' : 'L',
        autopull, pull_thresh, out_shiftdir ? 'R' : 'L');
    APIO_LOG("    fjoin: tx=%d rx=%d rx_put=%d rx_get=%d  in_count=%d",
        fjoin_tx, fjoin_rx, fjoin_rx_put, fjoin_rx_get, in_count);

    APIO_LOG("  PINCTRL: 0x%08lX", (unsigned long)pinctrl);
    APIO_LOG("    out: base=%d count=%d  set: base=%d count=%d  in_base=%d",
        out_base, out_count, set_base, set_count, in_base);
    APIO_LOG("    side: base=%d count=%d", sideset_base, sideset_count);

    APIO_LOG("  .program pio%d_sm%d", pio_block, pio_sm);
    for (int ii = first_instr; ii <= end; ii++) {
        if (ii == start) {
            APIO_LOG("  .start");
        }
        if (ii == wrap_bottom) {
            APIO_LOG("  .wrap_target");
        }
        apio_instruction_decoder(instr_scratch[ii], instr, first_instr);
        APIO_LOG("    %d: 0x%04X ; %s", ii - first_instr, instr_scratch[ii], instr);
        if (ii == wrap_top) {
            APIO_LOG("  .wrap");
        }
    }
}

#endif // APIO_LOG_ENABLE && APIO_LOG_IMPL

#endif // APIO_DIS_H