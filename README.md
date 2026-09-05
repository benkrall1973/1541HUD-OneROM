# DriveHUD-OneROM

**DriveHUD** is a passive Commodore 1541 drive monitor built on the [OneROM](https://onerom.org) platform.

This repository is a derivative/fork of **OneROM v0.7.1**, originally created and maintained by [Piers Finlayson](https://github.com/piersfinlayson). DriveHUD uses the OneROM Fire-24-E hardware and firmware/plugin architecture as its foundation, then adds Commodore 1541-specific passive monitoring, USB state transport, a desktop GUI, and a reproducible DriveHUD build path.

DriveHUD is **not the upstream OneROM project**. If you are looking for general OneROM hardware, firmware, documentation, purchasing information, or support, use the original project:

- [OneROM website](https://onerom.org)
- [Original OneROM GitHub repository](https://github.com/piersfinlayson/one-rom)

## What DriveHUD Does

DriveHUD monitors a Commodore 1541 while the drive is operating and presents live drive state in a Python desktop GUI.

The current hardware-proven **DriveHUD V0.0.30** supports:

- Track position
- Half-track position
- Diagnostic-cartridge half-step tracking
- Spindle motor ON/OFF state
- Head movement: IN / OUT / STALL / PARK
- HOME anchoring
- Write-protect state
- Hardware density selection D0-D3
- USB connection after the drive is already running
- GUI reconnect without requiring a drive reset
- RP2350 state caching and STATE resend

The monitor is intentionally **passive**. DriveHUD observes the 1541 bus but does not drive the 1541 bus.

## How DriveHUD Uses OneROM

DriveHUD is integrated into the OneROM firmware environment rather than being a completely separate application.

The relationship is roughly:

```text
OneROM v0.7.1
    |
    +-- OneROM Fire-24-E firmware and build infrastructure
    |
    +-- DriveHUD passive firmware configuration
    |
    +-- DriveHUD USER acquisition plugin
    |
    +-- DriveHUD-aware USB SYSTEM plugin
    |
    +-- DriveHUD Python GUI
    |
    +-- DriveHUD build and documentation
```

This repository keeps the OneROM source tree because DriveHUD firmware is built together with the OneROM base firmware and plugins to create the final Fire-24-E image. Keeping the fork intact also makes it possible to compare DriveHUD against future upstream OneROM versions and selectively incorporate upstream fixes without losing the hardware-proven DriveHUD baseline.

## Hardware Architecture

The proven configuration uses a **OneROM Fire-24-E** in a Commodore 1541.

- **UB3**: normal ROM-serving OneROM
- **UB4**: dedicated passive DriveHUD monitor

The UB4 DriveHUD instance is built with the passive configuration and must never drive the 1541 bus.

DriveHUD currently observes UC2 (the 1541's VIA2) and selected DOS RAM activity to derive drive state. Acquisition is performed in RP2350 PIO/DMA so the monitoring path does not depend on USB or GUI timing.

## Repository Layout

The original OneROM source tree remains present because it is the platform DriveHUD is built on. DriveHUD-specific files are concentrated in a small number of locations:

| Path | Purpose |
|---|---|
| [`drivehud/`](drivehud) | DriveHUD build script, documentation, and GUI |
| [`drivehud/gui/DriveHUD_V0.0.30_PIO_DMA.py`](drivehud/gui/DriveHUD_V0.0.30_PIO_DMA.py) | Hardware-proven DriveHUD GUI |
| [`plugins/user/drivehud-probe/`](plugins/user/drivehud-probe) | DriveHUD acquisition/decoder USER plugin |
| [`plugins/system/usb/`](plugins/system/usb) | OneROM USB SYSTEM plugin with DriveHUD mailbox support |
| [`firmware/src/piodma/pio.c`](firmware/src/piodma/pio.c) | Passive DriveHUD firmware integration |
| [`drivehud/Build-DriveHUD-V030.ps1`](drivehud/Build-DriveHUD-V030.ps1) | Canonical DriveHUD V0.0.30 build script |

For detailed DriveHUD build and reproducibility information, see [`drivehud/README.md`](drivehud/README.md).

## Versioning and Hardware-Proven Baseline

The tag **`v0.0.30`** identifies the hardware-proven DriveHUD V0.0.30 runtime baseline.

Later repository commits may contain documentation, CI, build portability, or release-engineering improvements for the same V0.0.30 runtime. Those changes do not automatically represent a new DriveHUD runtime version.

A new DriveHUD runtime version should be assigned only when runtime behavior changes and the new version has been tested appropriately on real hardware.

## Building DriveHUD

The canonical build is:

```text
drivehud/Build-DriveHUD-V030.ps1
```

The build process:

1. Generates the required 8 KB companion ROM.
2. Builds the OneROM v0.7.1 base firmware in passive DriveHUD configuration.
3. Builds the DriveHUD USER plugin.
4. Builds the USB SYSTEM plugin with DriveHUD mailbox support.
5. Composes the Fire-24-E firmware image with the OneROM CLI.
6. Converts the resulting BIN image to UF2.
7. Reports SHA256 hashes for the generated artifacts.

See [`drivehud/README.md`](drivehud/README.md) for the tested build environment, WSL support, reproducibility results, and known dependency caveats.

## Relationship to Upstream OneROM

DriveHUD-OneROM is based on **OneROM v0.7.1** and intentionally retains the upstream project history and source structure.

Upstream OneROM remains the authority for general OneROM development. DriveHUD-specific changes are maintained here for the Commodore 1541 monitoring project.

Future OneROM releases should be treated as upstream updates to evaluate, not changes to merge automatically. This protects the known-good DriveHUD hardware baseline while still allowing useful upstream fixes and improvements to be incorporated deliberately.

## Credits

**OneROM** was created by **Piers Finlayson**. DriveHUD would not exist in its current form without the OneROM Fire hardware, firmware architecture, plugin system, CLI tooling, and associated open-source work.

Original project:

- [github.com/piersfinlayson/one-rom](https://github.com/piersfinlayson/one-rom)
- [onerom.org](https://onerom.org)

DriveHUD is an independent derivative project focused specifically on passive Commodore 1541 monitoring.

## License

This repository retains the upstream OneROM licensing structure and notices. Software and firmware are licensed under the MIT License, while applicable hardware design files use the CERN Open Hardware Licence Version 2 - Weakly Reciprocal.

See [`LICENSE.md`](LICENSE.md) for the complete license terms and copyright notices.
