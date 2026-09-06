# 1541HUD-OneROM

**1541HUD** is a passive real-time Commodore 1541 drive monitor built on the [OneROM](https://onerom.org) platform.

The current stable release is **1541HUD V0.0.31**. The previous **DriveHUD V0.0.30** tag is preserved as the immutable hardware-proven baseline.

This repository is an independent derivative of **OneROM v0.7.1**, originally created and maintained by **Piers Finlayson**. It is not a GitHub fork and is not the upstream OneROM project. 1541HUD retains the OneROM source tree because the monitor is built together with the OneROM Fire-24-E firmware and plugin system.

For general OneROM hardware, firmware, documentation, purchasing information, or support, use the original project:

- [OneROM website](https://onerom.org)
- [Original OneROM GitHub repository](https://github.com/piersfinlayson/one-rom)

## What 1541HUD Does

1541HUD monitors a Commodore 1541 while the drive is operating and presents live drive state in a desktop GUI. The monitor is intentionally **passive**: it observes the 1541 bus but does not drive it.

The proven monitor architecture supports:

- Track and half-track position
- Diagnostic-cartridge half-step tracking
- Spindle motor ON/OFF state
- Head movement: IN / OUT / STALL / PARK
- HOME anchoring
- Write-protect state
- Hardware density selection D0-D3
- USB late connection and GUI reconnect
- RP2350 state caching and STATE resend

## Hardware Architecture

The established configuration uses a **OneROM Fire-24-E** in a Commodore 1541:

- **UB3**: normal ROM-serving OneROM
- **UB4**: dedicated passive 1541HUD monitor

Acquisition is performed with RP2350 PIO/DMA and is independent of USB or GUI timing.

```text
1541 bus activity
      |
      v
RP2350 PIO/DMA capture
      |
      v
1541HUD state decoder/cache
      |
      v
shared mailbox -> USB CDC -> Python GUI
```

## Repository Layout

| Path | Purpose |
|---|---|
| [`1541hud/`](1541hud) | 1541HUD build script, project documentation, changelog, release process, and GUI |
| [`1541hud/gui/1541HUD_V0.0.31_PIO_DMA.py`](1541hud/gui/1541HUD_V0.0.31_PIO_DMA.py) | Current stable GUI entry point |
| [`plugins/user/1541hud-probe/`](plugins/user/1541hud-probe) | Passive acquisition/decoder USER plugin |
| [`plugins/system/usb/`](plugins/system/usb) | USB SYSTEM plugin with 1541HUD mailbox transport |
| [`firmware/src/piodma/pio.c`](firmware/src/piodma/pio.c) | Passive firmware integration |
| [`1541hud/Build-1541HUD-V031.ps1`](1541hud/Build-1541HUD-V031.ps1) | Canonical V0.0.31 build script |

The rest of the repository largely remains the upstream OneROM v0.7.1 foundation required to build, test, and maintain 1541HUD. Upstream documentation and tooling are intentionally retained rather than copied into a separate vendored snapshot.

## Which Documentation to Use

For 1541HUD itself, use these files first:

- [`1541hud/README.md`](1541hud/README.md) for build, architecture, and reproducibility details.
- [`1541hud/CHANGELOG.md`](1541hud/CHANGELOG.md) for 1541HUD/DriveHUD release history.
- [`1541hud/RELEASE.md`](1541hud/RELEASE.md) for the 1541HUD release process.

The repository root `CHANGELOG.md`, much of `docs/`, and other retained OneROM material describe the upstream OneROM foundation. They remain for provenance, build support, and future upstream comparison; they are not the 1541HUD release history or release procedure.

## Versioning

- **`v0.0.30`**: immutable DriveHUD hardware-proven baseline.
- **`v0.0.31`**: current stable 1541HUD release and project rename baseline.
- Future development starts from V0.0.31 and should preserve the passive-monitor invariants unless a change is deliberately tested and documented.

Tags are not moved or rewritten after release.

## Relationship to Upstream OneROM

1541HUD-OneROM is based on **OneROM v0.7.1** and intentionally retains upstream history and source structure. Upstream OneROM remains the authority for general OneROM development.

Future upstream releases should be evaluated and incorporated deliberately rather than automatically merged into the stable 1541HUD line.

## Credits

**OneROM** was created by **Piers Finlayson**. 1541HUD depends on the OneROM Fire hardware, firmware architecture, plugin system, CLI tooling, and associated open-source work.

1541HUD is an independent derivative project focused specifically on passive Commodore 1541 monitoring.

## License

This repository retains the upstream OneROM licensing structure and notices. Software and firmware are licensed under the MIT License, while applicable hardware design files use the CERN Open Hardware Licence Version 2 - Weakly Reciprocal.

See [`LICENSE.md`](LICENSE.md) for the complete license terms and copyright notices.
