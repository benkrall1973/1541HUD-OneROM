# 1541HUD Fire-24G V8.1 Build and Installation Record

This records the active configuration for the paired OneROM installation in
the 1541 long board. Control core/plugins and Monitor operation were tested
in the drive. The exact public stock Control composition passed 47 loose-board
checks on 2026-11-10; its drive-operation and full-power-cycle qualification
remain pending. See the [baseline runbook](V81_BASELINE_RUNBOOK.md) for the
image-specific test scope.

UB3 is the state-changing **Control** board; UB4 is the electrically passive
**Monitor** board. Do not exchange their firmware, USB roles, or wiring.

## Confirmed board identity

| Board | Role | Active firmware |
| --- | --- | --- |
| UB3 | Control | Official v0.8.1; USB 0.8.1.16 / Host-Control 0.8.1.5 |
| UB4 | Monitor | 1541HUD Monitor `v0.8.1.32` |

Windows COM numbers are not identities and may change after every reconnect.
Bind 1541HUD by each board's USB serial as discovered locally; Windows COM
numbers are not identities and must not be documented as a deployment contract.

## Fire-24G GPIO assignment

| Board | OneROM signal / GPIO | 1541 endpoint | Purpose |
| --- | --- | --- | --- |
| UB3 | SEL_A / GPIO26 | existing J1 address-control wire | IEC device 8/9 control |
| UB3 | SEL_B / GPIO27 | C46 positive / CR7 anode through **1 kOhm** | active-low 1541 reset |
| UB3 | SEL_C / GPIO25 | 5.1 kOhm to KSC2383YTA base | temporary write-protect override |
| UB3 | SEL_D / GPIO24 | P6 pin 12 through 22 kOhm; 22 kOhm from SEL_D to GND | physical write-protect sense |
| UB3 | X1 / GPIO9 | 74LS138 pin 10, `/Y5` | `$A000-$BFFF` page select |
| UB3 | X2 / GPIO8 | 74LS138 pin 7, `/Y7` | `$E000-$FFFF` page select |
| UB4 | SEL_A / GPIO26 | UC2 pin 23, `/CS2` | passive VIA2 select capture |
| UB4 | SEL_B / GPIO27 | UC2 pin 17, PB7 / active-low SYNC | raw SYNC/RPM capture |
| UB4 | X1 / GPIO9 | UC4 pin 34, `R/W` | passive write-cycle qualification |
| UB4 | X2 / GPIO8 | UC4 pin 39, `phi2` | passive bus timing capture |

UB4 has **no write-protect wiring or write-protect authority**. Write-protect
override and physical status are UB3-only features.

### Reset and write-protect rules

- SEL_B is only ever driven LOW then released; it is never driven HIGH.
- The fitted 1 kOhm reset resistor is required. The former 2.2 kOhm value did
  not discharge C46 adequately and could leave IEC reads stalled after reset.
- `ROMRESET` holds SEL_B LOW for **350 ms**, then releases it. OneROM boot and
  reboot also hold the node LOW for 350 ms until ROM service is available.
- SEL_C reaches P6 pin 12 only through the 5.1 kOhm base resistor and NPN.
  Never connect a OneROM output directly to P6 pin 12 or UC2.
- SEL_D is 3.3 V input-only. The 22 kOhm / 22 kOhm divider is mandatory.

## Build entry points

All project source, composition manifests and final tested images are checked
into this branch. Generated object directories and third-party tool executables
are intentionally not committed; install the pinned tool prerequisites below.

| Component | Build command | Expected output |
| --- | --- | --- |
| UB3 Control, WSL2/Linux source build | `bash consolidation/scripts/build-control-v0.8.1.sh private-roms/<your-config>.json output/onerom-ub3-control-fire24g.bin` | `output/onerom-ub3-control-fire24g.bin` |
| UB4 Monitor, WSL2/Linux source build | `TOOLCHAIN=/path/to/bin ONEROM_CLI=onerom bash firmware-monitor/Build-Monitor-Fire24G-V0.8.1.sh` | `firmware-monitor/releases/v8.1/candidates/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.32.bin` |

Both wrappers use the root-level pinned source tree at `source/one-rom-v0.8.1`.
Both wrappers export the supported source into separate ignored work trees and verify its checked-in host fixes against the source manifest; see HOST_DEPENDENCY_OVERLAY.md. Firmware C source remains at the pinned baseline. Control then builds its core/plugins and composes the owner-supplied
manifest. Monitor additionally applies the passive core overlay
with `HUD1541_PASSIVE_UB4`; do not build its plugins with a normal ROM-serving core.

### UB4 Monitor pinned inputs

The public UB4 Monitor composition includes its baseline inputs. The official
USB Makefile downloads pinned TinyUSB ec528d6aa, picobootx v0.7.0 and
pico-sdkless v0.1.1 into the isolated core's `plugins/system/usb/` directory.
The Monitor manifest also uses
the two checked-in 8 KiB baseline inputs below; these are the baseline inputs
used to produce the supplied Monitor release image.

| File | SHA-256 |
| --- | --- |
| `firmware-monitor/inputs/1541-OneROM-Bootloader-Universal-v1.1.0.bin` | `7313B07F26FCC3778664B0C53F1AFF46AF883ED8B0A3CBF525EFC9BB40432B0E` |
| `firmware-monitor/inputs/1541-c000.325302-01.bin` | `6FA7B07AFF92DA66B0A28A52BB3C82FFE310AB0FAD2CC473B40137A8D299C7E5` |

The Monitor includes the stock C000 baseline page shown above.  Keep owner-supplied
DOS pages for UB3 Control private under `private-roms/`.

### Supported source-build host: WSL2 or Linux

The qualified source-build route is **WSL2/Linux** with GNU Make, ARM GNU
Toolchain 15.3.Rel1, and a POSIX shell.  The upstream V8.1 Makefiles use POSIX
recipe commands such as `mkdir -p`, pipes, and command substitution.  Plain
native Windows builds with GNU Make plus a loose `sh.exe` are not reliable and
are not a supported public route.

`firmware-monitor/Build-Monitor-Fire24G-V0.8.1.ps1` is retained as a Windows
WSL wrapper: toolchain and CLI arguments are Linux paths.
Windows remains fully supported
for the OneROM CLI, USB inspection/flashing, and the 1541HUD application.

### Prerequisites for a fresh checkout

- ARM GNU Toolchain 15.3.Rel1 or compatible `arm-none-eabi-gcc`.
- WSL2/Linux, GNU Make, and a POSIX shell for source builds.
- Git, patch, tar, Python 3 and native C/C++ build tools; network access for
  the pinned upstream dependencies and Cargo crates.
- OneROM CLI 0.6.0 (1541HUD hud1) or compatible `onerom` executable. The validated Windows
  executable is included at `tools/onerom-v0.6.0-hud1.exe`; place it on `PATH` or
  pass its path to the build wrapper where needed.
- For the patched archive/composition path, use the CLI built by the Control
  wrapper. WSL/Linux builds need a Linux CLI; the combined source-build checker
  builds and selects it automatically. The bundled Windows CLI is built with the same checked host overlay;
  see tools/windows-cli-manifest.json for its exact provenance.
- Rust/Cargo **1.99.0** (the CI-validated toolchain) for the pinned V8.1 Rust host tooling. The original 1.85 floor was only a wrapper check; it is not sufficient for the currently vendored source snapshot. The dependency-refresh branch validated `onerom-fw` and `onerom-cli` with Rust/Cargo 1.93.0 on Linux.

The Monitor PowerShell entry point also accepts `-Toolchain` and
`-OneRomCli` arguments for an installed-but-not-on-PATH toolchain. This keeps
the repository portable without embedding operator-specific paths.
`Build-Control-V8.1-Clean.cmd` remains an optional native Windows composition
helper for an already-built core and plugins. Invoke it as
`Build-Control-V8.1-Clean.cmd private-roms/<your-config>.json output/<name>.bin`.
It is not the supported fresh public source-build path.

## Flash and acceptance procedure

1. Shut down 1541HUD so it releases both CDC ports.
2. Use `onerom --unrecognised scan` and verify the target serial. Do not flash
   the other board; select the target explicitly by serial.
3. Program with `--verify`, reboot normally, and inspect the installed plugin
   versions/slots before reconnecting 1541HUD.
4. UB3: verify ROM save/readback, IEC 8 and 9 save/readback, write-protect
   OFF/ON/OFF, then `ROMRESET` followed by an immediate directory read.
5. UB4: verify green idle, a density colour while reading, physical-header and
   raw-SYNC RPM telemetry, and no effect on drive operation.

## Monitor RGB state code

During active motor capture, the LED represents current density: D3 yellow,
D2 blue, D1 orange, D0 white. Solid green is idle/waiting; amber blinking is a
recent capture/event overflow and clears after 1.5 seconds without a new
overflow; red fast blinking is a hardware/resource fault; purple is the short
boot heartbeat. While no HUD CDC client is connected, telemetry events are
discarded rather than accumulating into a false amber warning. Status states
override density.
