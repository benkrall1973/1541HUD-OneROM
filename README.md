# 1541HUD / OneROM Fire-24G

## Project introduction

1541HUD is a OneROM community project for Commodore 1541 owners who want
easier ROM switching, USB control, and optional live drive-status monitoring.
It begins with one compatible **UB3 Control** OneROM board and a practical
Windows interface, using the drive ROMs that you are entitled to use. From
there, the installation can grow at your own pace, including an optional small
Raspberry Pi touchscreen display mounted with the drive.

The current Fire-24G baseline provides several paths:

- Flash the supplied stock UB3 image for a safe Control/USB starting point.
- Use 1541HUD on Windows for ROM selection, IEC device 8/9 selection, reset,
  UB3 write-protect sense/override, and LED policy.
- Add a second, passive **UB4 Monitor** board for live telemetry on motor
  activity, full- and half-track position, stepping activity, observed
  write-gate/write-pulse activity,
  disk density, headers, RPM, and SYNC activity. Unlike a simple activity
  light, the passive monitor can decode checksum-validated physical GCR
  headers, show sector coverage and recent header order, estimate disk identity,
  correlate density with SYNC-per-revolution, and expose capture-health/drop
  counters alongside the drive's observed mechanical state.
- Compose custom Lower, Upper, or C/A/E ROM layouts from lawful ROM pages with
  the included templates and build documentation. A RAMBoard-capable C/A/E
  installation can support owner-supplied SpeedDOS, JiffyDOS, and DolphinDOS
  2.0 ROM sets where the required hardware and lawful ROM pages are present.

The goal is a useful baseline that the community can build on: additional
qualified ROM layouts, monitor cards, diagnostics, display options, and
hardware profiles can be proposed, tested, documented, and contributed over
time.

> **Project note:** This is a community builder/tester project. I use AI as a
> practical development partner for research, coding, documentation, test
> ideas, and troubleshooting, while learning and validating the hardware as I
> go.

This repository is the public Fire-24G baseline built from a pinned,
**unreleased** OneROM V8.1 development snapshot from
[Piers Finlayson's upstream `releases/0.8.1` branch](https://github.com/piersfinlayson/one-rom/tree/releases/0.8.1).
It is a community-tested integration, not an official upstream OneROM V8.1
release. It includes qualified source, wiring records, the 1541HUD Python
application, ROM-free templates, and two ready-to-flash stock images. It
deliberately does **not** distribute third-party DOS ROMs.

## AI-assisted builds

An AI coding assistant can be a practical build partner for this project: it
can inspect the repository, help install and verify the documented toolchain,
prepare a lawful ROM manifest, run the qualified build wrapper, compare hashes,
and troubleshoot a failed build. The repository is deliberately organized so a
fresh clone contains the pinned official source, runbook, and validation
material needed for that work.

AI assistance does not replace builder judgment. Supply only ROM pages you are
entitled to use, review any changes before building, and verify the target
OneROM serial before flashing. Never let an assistant guess which of two
connected boards should be programmed.

## Before you start

This is a documented modification for a 1541 long board, not a universal
drop-in firmware update. Read the [verified wiring guide](docs/FIRE24G-VERIFIED-WIRING.md)
before connecting a board to the drive. In particular:

- **Control and Monitor are roles, not different Fire-24G hardware.** The same
  Fire-24G board can be flashed with either image, but a **UB3 Control**
  installation serves the drive ROM and controls ROM selection, IEC address,
  reset, and optional write protect; a **UB4 Monitor** installation is wired
  only for passive telemetry. Flash the image that matches the intended role
  and its wiring. Do not use a Monitor image where the Control image is
  required, or vice versa.
- For a standard two-ROM 1541 with **no 74LS138**, `X1` **must** drive pin 20
  (`/CS`) of the empty ROM socket. For the usual lower-socket installation,
  fit OneROM in the `$C000` **UB3 socket** and wire `X1` to pin 20 of the
  `$E000` **UB4 socket**. Those are main-PCB socket references, not the
  optional UB4 Monitor board.
- The supplied UB3 stock image supports IEC **8/9**, 350 ms active-low drive
  reset, and optional write-protect control.
- The verified reset circuit uses UB3 `SEL_B` through a **1 kOhm** resistor to
  the C46-positive/CR7-anode reset node. Do not connect it directly to UC4
  pin 40.
- Optional write-protect override uses `SEL_C` through a **5.1 kOhm** resistor
  to the base of a **KSC2383YTA NPN** transistor; connect its emitter to 1541
  ground and its collector to **P6 pin 12**. This transistor is required:
  never wire a OneROM GPIO directly to P6 pin 12.
- Optional physical write-protect sensing is separate: route **P6 pin 12**
  through **22 kOhm** to `SEL_D`, and fit a second **22 kOhm** resistor from
  `SEL_D` to ground. `SEL_D` is input-only for this purpose; never connect P6
  pin 12 directly to a OneROM GPIO.
- UB4 is a passive monitor only. It must not be used to control write protect
  or serve the drive ROM.
- ROM and IEC Apply+Save were tested with a normal powered USB cable. A full
  USB power cycle is no longer required for those changes. Drive power-off
  detection remains a separate, deferred feature.

## What is included

| Item | Purpose |
| --- | --- |
| `releases/` | Ready-to-flash Fire-24G UB3 stock Control and UB4 Monitor images. |
| `source/one-rom-v0.8.1/` | Exact signed official OneROM v0.8.1 source used by both build roles. |
| `tools/onerom-v0.5.0.exe` | Bundled upstream OneROM CLI for Windows scan, inspection, and flashing. |
| `1541hud/` | 1541HUD Python source, launcher, build script, and tests. |
| `firmware-control/` | UB3 Control source and 1541HUD Control plugins. |
| `firmware-monitor/` | UB4 passive Monitor source and plugins. |
| `templates/` | ROM-free templates for owner-supplied Lower, Upper, C/A/E, and Monitor inputs. |
| `docs/` | Wiring, engineering, build, release, and ROM-preparation references. |

## Quick start: flash the supplied images

The included OneROM CLI avoids a separate download for the stock-image
workflow. Close 1541HUD first so it releases the board USB ports.

1. Connect **only the intended OneROM** and put it into BOOTSEL.
2. Open PowerShell in the repository root and discover the board:

   ```powershell
   ./tools/onerom-v0.5.0.exe --unrecognised --board fire-24-g scan
   ```

3. Confirm the displayed serial is the board you intend to flash.
4. Flash exactly one image, replacing `<SERIAL>` with that board's serial.

   **UB3 Control - stock DOS 2.6 test image**

   ```powershell
   ./tools/onerom-v0.5.0.exe --serial <SERIAL> program `
     --firmware ./releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-test-v0.8.1.bin `
     --verify
   ```

   **UB4 Monitor - passive telemetry image**

   ```powershell
   ./tools/onerom-v0.5.0.exe --serial <SERIAL> program `
     --firmware ./releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.30.bin `
     --verify
   ```

5. Reboot normally and confirm USB enumeration. Test UB3 first: IEC 8/9,
   reset, then an immediate directory read. Install and test UB4 only after
   the Control board is known good.

See [public release images](docs/PUBLIC_FIRE24G_RELEASES.md) for SHA-256
values, image contents, and the full acceptance sequence.

## Run 1541HUD

On Windows, run:

```text
1541hud/source/Run-1541HUD-V0.8.0.51.cmd
```

The application discovers OneROM boards by USB serial, not by an unstable COM
number. Assign the Control and Monitor roles in **Settings**, then use the
dashboard cards to inspect telemetry or issue Control commands. A connected
UB4 is optional; UB3 remains usable for ROM, IEC, reset, and write-protect
functions without it.

## Custom ROM images

The supplied firmware is intentionally a stock test baseline. To build a
custom DOS image, use ROM files that you are legally entitled to use:

1. Read the [ROM page preparation guide](docs/THIRD_PARTY_DOS_PAGE_PREPARATION.md).
2. Select the matching template in `templates/`:
   - Lower socket: C/E pages
   - Upper socket: E/C pages
   - C/A/E: three-page layout for the documented 74LS138 wiring
3. Copy the template and your ROM pages into ignored `private-roms/` paths.
4. Follow the [UB3 Control composition guide](docs/UB3_CONTROL_CLI_COMPOSITION_GUIDE.md).

Do not commit ROM images, completed private manifests, disk images, or custom
firmware binaries back to a public fork.

## Building from source

The qualified source-build host is WSL2 or Linux with ARM GNU Toolchain 14.2.1,
GNU Make, and a POSIX shell; Rust/Cargo is also required for Control builds.
Native Windows remains supported for 1541HUD and the bundled OneROM CLI, which
is sufficient for inspecting and flashing release images, but it is not a
replacement for the compiler toolchain.

The detailed procedures and build-script arguments are in
[Fire-24G V8.1 Build and Installation](docs/FIRE24G_V81_BUILD_AND_INSTALL.md).
Run the package check before publishing a derivative:

```powershell
./tools/maintenance/Test-Public-Package.ps1 -PackageRoot .
```

## Documentation map

- [Release images and hashes](docs/PUBLIC_FIRE24G_RELEASES.md)
- [Verified wiring and reset behavior](docs/FIRE24G-VERIFIED-WIRING.md)
- [Engineering dossier: GPIO, LEDs, reset, architecture](docs/FIRE24G_V81_ENGINEERING_DOSSIER.md)
- [Build and installation runbook](docs/FIRE24G_V81_BUILD_AND_INSTALL.md)
- [UB3 custom composition guide](docs/UB3_CONTROL_CLI_COMPOSITION_GUIDE.md)
- [ROM page preparation](docs/THIRD_PARTY_DOS_PAGE_PREPARATION.md)
- [Package boundary and redistribution policy](docs/PUBLIC_PACKAGE_MANIFEST.md)
- [Release checklist](docs/RELEASE_CHECKLIST.md)
- [Validation matrix and support boundary](docs/VALIDATION_MATRIX.md)
- [Dependency-alert triage](docs/DEPENDENCY_ALERT_TRIAGE.md)
- [Notices and upstream provenance](NOTICE.md)
- [Contributing guide](CONTRIBUTING.md)
- [Security and hardware-safety reporting](SECURITY.md)

## Safety and support boundaries

Disconnect power before altering wiring. Never connect a 5 V drive signal
directly to a 3.3 V-only OneROM GPIO. Do not flash a board until its serial
and role are known. This project documents a specific, tested installation;
other 1541 revisions and wiring choices require their own electrical review.

Contributions are welcome under the repository [MIT License](LICENSE). Preserve
the separate licenses and notices for OneROM and all vendored dependencies;
the project does not grant redistribution rights for third-party DOS ROMs.


