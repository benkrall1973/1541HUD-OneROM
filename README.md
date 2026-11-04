# 1541HUD / OneROM Fire-24G

## Project introduction

1541HUD is a OneROM community project for Commodore 1541 owners who want
easier ROM switching, USB control, and optional live drive-status monitoring.
It begins with one compatible **UB3 Control** OneROM board and a Windows
interface, using the drive ROMs that you are entitled to use. From there, the
installation can grow at your own pace.

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

The project is designed to work as a practical Windows-based tool today and
can also support a small Raspberry Pi touchscreen display mounted with the
drive. Firmware, setup guides, source, and testing material are available in
this repository: [github.com/benkrall1973/1541HUD-OneROM](https://github.com/benkrall1973/1541HUD-OneROM).

The goal is a useful baseline that the community can build on: additional
qualified ROM layouts, monitor cards, diagnostics, display options, and
hardware profiles can be proposed, tested, documented, and contributed over
time.

> **Project note:** This is a community builder/tester project. I use AI as a
> practical development partner for research, coding, documentation, test
> ideas, and troubleshooting, while learning and validating the hardware as I
> go.

This repository is the public V8.1 Fire-24G baseline. It includes qualified
source, wiring records, the 1541HUD Python application, ROM-free templates,
and two ready-to-flash stock images. It deliberately does **not** distribute
third-party DOS ROMs.

## Before you start

This is a documented modification for a 1541 long board, not a universal
drop-in firmware update. Read the [verified wiring guide](docs/FIRE24G-VERIFIED-WIRING.md)
before connecting a board to the drive. In particular:

- UB3 and UB4 run different images and are not interchangeable.
- The supplied UB3 stock image supports IEC **8/9**, 350 ms active-low drive
  reset, and optional write-protect control.
- The verified reset circuit uses UB3 `SEL_B` through a **1 kOhm** resistor to
  the C46-positive/CR7-anode reset node. Do not connect it directly to UC4
  pin 40.
- UB4 is a passive monitor only. It must not be used to control write protect
  or serve the drive ROM.

## What is included

| Item | Purpose |
| --- | --- |
| `releases/` | Ready-to-flash Fire-24G UB3 stock Control and UB4 Monitor images. |
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

   **UB3 Control — stock DOS 2.6 test image**

   ```powershell
   ./tools/onerom-v0.5.0.exe --serial <SERIAL> program `
     --firmware ./releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-test-v0.8.1.bin `
     --verify
   ```

   **UB4 Monitor — passive telemetry image**

   ```powershell
   ./tools/onerom-v0.5.0.exe --serial <SERIAL> program `
     --firmware ./releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.28.bin `
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

The qualified Windows toolchain is ARM GNU Toolchain 14.2.1, GNU Make, and a
Windows-safe BusyBox `sh.exe`; Rust/Cargo is also required for Control builds.
The bundled CLI is sufficient for flashing release images, but it is not a
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

## Safety and support boundaries

Disconnect power before altering wiring. Never connect a 5 V drive signal
directly to a 3.3 V-only OneROM GPIO. Do not flash a board until its serial
and role are known. This project documents a specific, tested installation;
other 1541 revisions and wiring choices require their own electrical review.
