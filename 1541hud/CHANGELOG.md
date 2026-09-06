# 1541HUD Changelog

This changelog tracks 1541HUD/DriveHUD project releases. The repository root `CHANGELOG.md` is retained as upstream OneROM history.

## v0.0.31 - 2026-09-05

Current stable 1541HUD release and project rename baseline.

- Renamed the public project from DriveHUD to 1541HUD.
- Preserved the proven V0.0.30 acquisition and USB mailbox protocol behavior.
- Added canonical 1541HUD source paths, GUI entry point, build script, and documentation.
- Preserved the immutable `v0.0.30` hardware-proven tag.
- Confirmed CI and 1541HUD source sanity checks on the released commit.
- Established `main` as the stable V0.0.31 public branch baseline.

No experimental V0.0.32+ work is part of this release.

## v0.0.30 - 2026-09-05

Hardware-proven DriveHUD baseline.

Confirmed behavior includes:

- Track and half-track display
- Diagnostic-cartridge manual half-step tracking
- Motor ON/OFF
- Head IN / OUT / STALL / PARK
- HOME anchoring
- Write-protect state
- Density D0-D3
- GUI late connection and reconnect
- RP2350 state caching and STATE resend

The `v0.0.30` tag is historical and immutable.
