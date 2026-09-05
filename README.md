# 1541HUD-OneROM

**1541HUD** is a passive real-time Commodore 1541 drive monitor built on the [OneROM](https://onerom.org) platform.

This repository is a derivative/fork of **OneROM v0.7.1**, originally created and maintained by **Piers Finlayson**. 1541HUD uses the OneROM Fire-24-E hardware, firmware, plugin architecture, and build tooling as its foundation, then adds Commodore 1541-specific passive monitoring, USB state transport, a Python desktop GUI, and a reproducible build path.

1541HUD is **not the upstream OneROM project**. For general OneROM hardware, firmware, documentation, purchasing information, or support, use the original project:

- [OneROM website](https://onerom.org)
- [Original OneROM GitHub repository](https://github.com/piersfinlayson/one-rom)

## What 1541HUD Does

1541HUD monitors a Commodore 1541 while the drive is operating and presents live drive state in a desktop GUI.

The hardware-proven baseline, released under the project's former **DriveHUD** name as **V0.0.30**, supports:

- Track and half-track position
- Diagnostic-cartridge half-step tracking
- Spindle motor ON/OFF state
- Head movement: IN / OUT / STALL / PARK
- HOME anchoring
- Write-protect state
- Hardware density selection D0-D3
- USB late connection and GUI reconnect
- RP2350 state caching and STATE resend

The monitor is intentionally **passive**. It observes the 1541 bus but does not drive the 1541 bus.

## How 1541HUD Uses OneROM

```text
OneROM v0.7.1
    |
    +-- Fire-24-E firmware and build infrastructure
    +-- 1541HUD passive firmware configuration
    +-- 1541HUD USER acquisition plugin
    +-- USB SYSTEM plugin with 1541HUD mailbox transport
    +-- 1541HUD Python GUI
    +-- 1541HUD build and documentation
```

The complete OneROM source tree remains in this repository because 1541HUD firmware is built together with the OneROM base firmware and plugins to produce the final Fire-24-E image. Keeping the upstream history also makes future OneROM releases easy to compare and selectively integrate without sacrificing the known-good 1541HUD baseline.

## Hardware Architecture

The proven configuration uses a **OneROM Fire-24-E** in a Commodore 1541:

- **UB3**: normal ROM-serving OneROM
- **UB4**: dedicated passive 1541HUD monitor

The UB4 monitor must remain passive. Acquisition is performed with RP2350 PIO/DMA and is independent of USB or GUI timing.

## Repository Layout

| Path | Purpose |
|---|---|
| [`1541hud/`](1541hud) | 1541HUD build script, documentation, and GUI |
| [`1541hud/gui/1541HUD_V0.0.31_PIO_DMA.py`](1541hud/gui/1541HUD_V0.0.31_PIO_DMA.py) | Current rename candidate GUI source |
| [`plugins/user/1541hud-probe/`](plugins/user/1541hud-probe) | Passive acquisition/decoder USER plugin |
| [`plugins/system/usb/`](plugins/system/usb) | OneROM USB SYSTEM plugin with 1541HUD mailbox transport |
| [`firmware/src/piodma/pio.c`](firmware/src/piodma/pio.c) | Passive firmware integration |
| [`1541hud/Build-1541HUD-V031.ps1`](1541hud/Build-1541HUD-V031.ps1) | Canonical candidate build script |

See [`1541hud/README.md`](1541hud/README.md) for build and reproducibility details.

## Versioning

The existing tag **`v0.0.30`** remains the immutable, hardware-proven DriveHUD baseline. It is intentionally not moved or rewritten as part of the project rename.

The 1541HUD rename work is being prepared as a **V0.0.31 candidate** because source/build identifiers have changed. V0.0.31 should not be called hardware-proven until the renamed build passes CI and is verified on a real 1541.

## Relationship to Upstream OneROM

1541HUD-OneROM is based on **OneROM v0.7.1** and intentionally retains the upstream project history and source structure. Upstream OneROM remains the authority for general OneROM development.

Future upstream releases should be evaluated and incorporated deliberately rather than automatically merged into the hardware-proven 1541HUD line.

## Credits

**OneROM** was created by **Piers Finlayson**. 1541HUD would not exist in its current form without the OneROM Fire hardware, firmware architecture, plugin system, CLI tooling, and associated open-source work.

Original project:

- [github.com/piersfinlayson/one-rom](https://github.com/piersfinlayson/one-rom)
- [onerom.org](https://onerom.org)

1541HUD is an independent derivative project focused specifically on passive Commodore 1541 monitoring.

## License

This repository retains the upstream OneROM licensing structure and notices. Software and firmware are licensed under the MIT License, while applicable hardware design files use the CERN Open Hardware Licence Version 2 - Weakly Reciprocal.

See [`LICENSE.md`](LICENSE.md) for the complete license terms and copyright notices.
