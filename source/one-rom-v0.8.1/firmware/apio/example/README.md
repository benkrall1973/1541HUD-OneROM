# apio Example

This completely bare metal example demonstrates how to use `apio` to build and run a simple PIO program on the RP2350.  It toggles GPIO0 at around 10 or 20Hz, depending on a random bit from the RP2350's TRNG at runtime.

## Build

To build, from the root of the repository:

```bash
make
```

If the arm-none-eabi toolchain is not in `/usr/bin`, set `TOOLCHAIN`:

```bash
export TOOLCHAIN=/path/to/arm-none-eabi-toolchain
make
```

You can download the toolchain from [ARM](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) or install it using your package manager, e.g. `apt install gcc-arm-none-eabi`.

## Run

Use either `build/example.elf` or `build/example.bin` to flash the RP2350.

If you have `picotool` installed, you can build and flash using the UF2:

```bash
make flash
```

## Logging

The example provides RTT trace which can be viewed using a debug (SWD) probe.  For example, using `probe-rs` to flash and connect to the RTT:

```bash
probe-rs run --chip rp2350 build/example.elf
```

## Files

The example program is made up of the following files:

- [`main.c`](main.c) - Example code using `apio` to build and run a simple PIO program.
- [`vector.c`](vector.c) - Reset vectors.
- [`boot.c`](boot.c) - Boot block.
- [`linker.ld`](linker.ld) - Linker script.