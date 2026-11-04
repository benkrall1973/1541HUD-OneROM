# Changelog

## v0.3.0 - 2026-08-20

**Breaking**: `APIO_RXF` no longer yields something you can assign to.  It is a
read, which is what it always was on a device - RXFn is read-only to the
processor, so a write through it never reached the hardware.  Under emulation it
did the opposite, appending a word as though a state machine had produced one,
so the same name moved data in opposite directions depending on the build.  A
write is now a compile error in both, rather than silently doing nothing on one
and something unrelated on the other.  To place a word in an emulated RX FIFO,
tell the emulator - epio's `epio_push_rx_fifo()` does it.

Added:

- `APIO_TXF_AT(BLOCK, SM)` and `APIO_RXF_AT(BLOCK, SM)`, which reach a named
  state machine's FIFO.  `APIO_TXF` and `APIO_RXF` address only the SM the
  assembler is currently building, and reaching another means `APIO_SET_SM`,
  which resets that SM's program bookkeeping.  That is right while building a
  program and wrong for code feeding a state machine that is already running.
  `APIO_TXF` and `APIO_RXF` are now defined in terms of these.
- `tx_fifo_overflow` in the emulation state, counting the words a full TX FIFO
  discarded, so a test can assert the discard rather than infer it.

Fixed:

- The emulated `APIO_TXF` wrote at `tx_fifo_count++` with no bound, running past
  the end of the FIFO row once more words were written than the FIFO is deep.
  It now discards the word and counts it, which is what a device does with a
  write to a full FIFO.

## v0.2.1 - 2026-08-10

Fixed:

- The state machine dump in `apio_dis.h` passed `uint32_t` values to `%08X`,
  which expects `unsigned int`.  `uint32_t` is `unsigned long` on
  arm-none-eabi, so the output was correct and nothing misbehaved, but a
  consumer that gives its logging function a `format(printf)` attribute - as
  One ROM now does - cannot build against it.  The values are cast, and the
  conversions qualified, so the format is right under both that ABI and a
  64 bit host where `uint32_t` is `unsigned int`.

## v0.2.0 - 2026-06-28

Covers the changes dated 2026-06-10 to 2026-06-28, detailed in the entries below.

**Breaking**: `APIO_GPIO_INIT()` now initialises all pins to the RP2350 hardware
reset defaults - pull-down enabled, 4 mA drive, slow slew.  Code written against
v0.1.0, where no pull state was tracked, may behave differently without any
compiler diagnostic.  The rename of `APIO_GPIO_OUTPUT` to
`APIO_GPIO_INPUT_OUTPUT` is *not* breaking - `APIO_GPIO_OUTPUT` is retained as a
synonym.

Added:

- Pad configuration macros: `APIO_GPIO_PULL_UP`, `APIO_GPIO_PULL_DOWN`,
  `APIO_GPIO_PULL_NONE`, `APIO_GPIO_DRIVE`, `APIO_GPIO_SLEW_FAST`,
  `APIO_GPIO_SLEW_SLOW`, `APIO_GPIO_INPUT_ONLY`.
- `APIO_ASM_CONTINUE()`, to modify PIOs after the initial setup.
- Block and SM addressing: `APIO_INSTR_COUNT`, `APIO_SET_BLOCK_VAR`,
  `APIO_SET_BLOCK_FROM`, `APIO_SET_BLOCK_FROM_VAR`, `APIO_SET_SM_VAR`,
  `APIO_END_BLOCK_FROM`.
- Register logging in `APIO_SM_LOG`.

## 2026-06-28

Add `APIO_ASM_CONTINUE()` to allow subsequent modification of PIOs after the initial setup.

## 2026-06-22

Handle INPUT_OUTPUT in emulator properly

## 2026-06-18

Re-add `APIO_GPIO_OUTPUT` as synonym for `APIO_GPIO_INPUT_OUTPUT`

## 2026-06-18

Correctly handle input/output state using `APIO_GPIO_INPUT_ONLY` and `APIO_GPIO_INPUT_OUTPUT` macros.

## 2026-06-16

Added support for emulating internal GPIO pulls.

- Renamed `APIO_GPIO_OUTPUT` to `APIO_GPIO_INPUT_OUTPUT` to accurately reflect
  that configuring a pin for PIO output control does not disable its input path.
- Added `APIO_GPIO_INPUT_ONLY(PIN)`: configures a pin as a pure SIO input with
  output driver disabled.  On real hardware, resets GPIO_CTRL to SIO and sets
  PAD_INPUT | PAD_OUTPUT_DIS.  No-op in emulation (tracked via `input_only`
  bitmask; EPIO will assert on any attempt to drive the pin as an output).
- Added `APIO_GPIO_PULL_UP(PIN)`, `APIO_GPIO_PULL_DOWN(PIN)`,
  `APIO_GPIO_PULL_NONE(PIN)`: configure pad pull resistors.  On real hardware,
  manipulate PUE/PDE bits in the pad register.  In emulation, set/clear
  `pull_up` / `pull_down` bitmasks in `_apio_emulated_gpios`.
- Added `APIO_GPIO_DRIVE(PIN, STRENGTH)` with constants `APIO_DRIVE_2MA`,
  `APIO_DRIVE_4MA`, `APIO_DRIVE_8MA`, `APIO_DRIVE_12MA`: configure pad drive
  strength.  On real hardware, sets DRIVE field [5:4] in the pad register.
  In emulation, stored per-pin (no behavioural effect currently).
- Added `APIO_GPIO_SLEW_FAST(PIN)` and `APIO_GPIO_SLEW_SLOW(PIN)`: configure
  pad slew rate.  On real hardware, sets/clears SLEWFAST in the pad register.
  In emulation, stored per-pin (no behavioural effect currently).
- Added fields to `_apio_emulated_gpio_t`: `pull_up` (bitmask), `pull_down`
  (bitmask), `input_only` (bitmask), `drive_strength[APIO_MAX_GPIOS]` (per-pin
  array), `slew_fast` (bitmask).
- `APIO_GPIO_INIT()` now initialises all new fields to their RP2350 hardware
  reset defaults: pull-down enabled on all pins, 4 mA drive strength, slow
  slew.

## 2026-06-14

Added register logging to APIO_SM_LOG

## 2026-06-10

Added APIs:
- APIO_INSTR_COUNT() — returns the number of instructions stored for the current block
- APIO_SET_BLOCK_VAR(BLOCK) — runtime variable equivalent of APIO_SET_BLOCK
- APIO_SET_BLOCK_FROM(BLOCK, OFFSET) — resume a previously committed block from a known offset
- APIO_SET_BLOCK_FROM_VAR(BLOCK, OFFSET) — runtime variable equivalent of APIO_SET_BLOCK_FROM
- APIO_SET_SM_VAR(SM) — runtime variable equivalent of APIO_SET_SM
- APIO_END_BLOCK_FROM(OFFSET) — commits only instructions added since APIO_SET_BLOCK_FROM

## 2026-02-24

Renamed `APIO_GPIO_INVERT` to `APIO_GPIO_INPUT_INVERT`, and added two new macros for forcing input pins to a specific value: `APIO_GPIO_FORCE_INPUT_LOW` and `APIO_GPIO_FORCE_INPUT_HIGH`.  Each macro clears the other settings.
