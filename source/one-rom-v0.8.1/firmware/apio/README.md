# apio - A Runtime RP2350 PIO Assembler and Disassembler

PIO programs are typically fixed at build time, using Raspberry Pi's `pioasm` PIO assembler, with the assembled code included in the firmware source.  However, there are cases where you need to generate PIO programs dynamically at runtime, using boot configuration or other program state to determine the PIO program configuration.

`apio` allows you to write PIO programs and configure the PIOs in C within your RP2350 firmware, using a simple [macro-based API](#usage).  This allows you to generate PIO programs dynamically at runtime, without needing to pre-assemble them at build time.

Even when PIO programs are fixed at build time, `apio` simplifies the build pipeline, by eliminating the need for a separate `pioasm` assembly step.

`apio` can be used in completely bare metal environments, with no Pico SDK (although it will work alongside the SDK).  It has very minimal [requirements](#requirements), with no external libraries required.

The included [disassembler](#disassembler) is useful for debugging and verification, allowing you to dump the assembled PIO program(s) at runtime.

`apio` can be integrated with [`epio`](https://github.com/piersfinlayson/epio) for PIO program [emulation](#emulation) on non-RP2350 hosts, including CI runners.

## Usage

See [the example](example/README.md) for a complete, minimal, bare-metal RP2350 program using `apio`.  Here is a snippet that:
- Initializes the assembler
- Defines a simple PIO program
- Configures and enables it

```c
APIO_ASM_INIT();                // Initialize assembler
APIO_CLEAR_ALL_IRQS();          // Clear any pending PIO IRQs

APIO_SET_BLOCK(0);              // Select PIO block 0
APIO_SET_SM(0);                 // Select state machine 0

uint8_t pause_count = RANDOM_BIT ? 15 : 30; // Choose between two delay values at runtime

APIO_ADD_INSTR(APIO_SET_PIN_DIRS(1));   // Set pin as output
APIO_WRAP_BOTTOM();                     // Set .wrap_bottom to current instruction address
APIO_ADD_INSTR(APIO_ADD_DELAY(APIO_SET_PINS(1), pause_count));  // Drive pin high, and pause
APIO_WRAP_TOP();                        // Set .wrap_top to current instruction address
APIO_ADD_INSTR(APIO_ADD_DELAY(APIO_SET_PINS(0), pause_count));  // Drive pin low, and pause

APIO_SM_CLKDIV_SET(150, 0);     // Set clock divider to 150.0
APIO_SM_EXECCTRL_SET(0);        // No EXECCTRL features enabled
APIO_SM_SHIFTCTRL_SET(0);       // No SHIFTCTRL settings required
APIO_SM_PINCTRL_SET(            // One output pin starting at GPIO 0
    APIO_SET_BASE(0) |
    APIO_SET_COUNT(1)
);
APIO_SM_JMP_TO_START();         // Configure SM to jump to start of program

APIO_END_BLOCK();               // Writes PIO0 instructions to hardware

APIO_ENABLE_SMS(0, (1 << 0));   // Enable PIO0 SM0
```

## Disassembler

The [example](example/README.md) produces the following output when connected to a debug probe, disassembling the PIO program above:

```text
PIO0:0 Example SM (3 instructions)
CLKDIV: 15000.00 EXECCTRL: 0x00002080 SHIFTCTRL: 0x00000000 PINCTRL: 0x04000000
.program pio0_sm0
.start
  0: 0xE081 ; set pindirs, 1
.wrap_target
  1: 0xFF01 ; set pins, 1 [30]
  2: 0xFF00 ; set pins, 0 [30]
.wrap
```

## Features

- Assemble and disassemble PIO programs dynamically at runtime.
- No heap or static data usage, suitable for minimal embedded environments.
- Pico SDK not required, supports fully bare metal programs.
- Disassembler can decode and dump programs at runtime for debugging and verification.
- Integrates with [`epio`](https://github.com/piersfinlayson/epio) for seamless PIO program emulation on non-RP2350 hosts, including CI runners.
- `apio` emulation mode is comprehensively tested by `epio`'s test suite, which includes hundreds of PIO programs covering all instructions and features.

## Requirements

`apio` is designed for completely bare metal environments, with no Pico SDK (although it will work alongside the SDK).  It has very minimal requirements:

### RP2350

- Suitable ARM compiler, e.g. `arm-none-eabi-gcc`.
- ~128 bytes of stack at runtime, during assembly.

### Hosted Environments

- Suitable C compiler, e.g. `gcc`.
- Minimal global data usage, for stored PIO programs.

## Versioning

`apio` follows [semantic versioning](https://semver.org/), with each release tagged in git.  While the major version is 0, breaking changes are released in a new minor version, and additions and fixes in a new patch version.  See [CHANGELOG.md](CHANGELOG.md) for what changed in each release.

`main` is not a release branch, and may take breaking changes at any time.  Pin your project to a tag:

```bash
git clone --depth 1 --branch v0.3.0 https://github.com/piersfinlayson/apio.git
```

If your project clones `apio` from a `Makefile`, keep the version in a variable, so it is visible and easy to bump:

```make
APIO_VERSION ?= v0.3.0

apio:
	@if [ ! -d "$@" ]; then \
		git -c advice.detachedHead=false clone --quiet --depth 1 \
			--branch $(APIO_VERSION) https://github.com/piersfinlayson/apio.git; \
	fi
```

Checking out a tag leaves the repository with a detached `HEAD`, which is what you want for a vendored copy - the contents are then frozen at that release.  The `-c advice.detachedHead=false` just suppresses git's advice message about it, to keep the build output readable.

Note that this target will not update an existing checkout - after bumping `APIO_VERSION`, remove the `apio/` directory so it is re-cloned.

## Emulation

`apio` integrates with [`epio`](https://github.com/piersfinlayson/epio) for seamless PIO program emulation on non-RP2350 hosts, including CI runners.

When using with `epio`:

- Compile your firmware with `APIO_EMULATION` defined before `apio.h` is included.  This is most easily done by passing `-DAPIO_EMULATION` to the compiler.
- One source file including `apio.h` must have `#define APIO_EMU_IMPL 1` before the header include, to include a single instance of the emulation capability.
- Once your `apio` assembler code has run, call `epio_from_apio()` to load `epio` with the assembled PIO programs and configuration.

You can then use `epio`'s API to run the PIOs.

## Contributions

Some PIO instructions are not yet implemented.  Adding these is straightforward - see [`apio.h`](include/apio.h).  Please submit a PR if you need an instruction that isn't implemented yet, or if you'd like to contribute in any other way.

## License

MIT License, see [LICENSE](LICENSE).
