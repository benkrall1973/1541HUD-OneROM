# 1541HUD Release Documentation

This repository is an independent derivative of OneROM v0.7.1. The upstream OneROM source tree is intentionally retained, but the upstream OneROM publishing procedure is **not** the release process for this project.

For 1541HUD releases, use:

- [`1541hud/RELEASE.md`](1541hud/RELEASE.md) for the release procedure.
- [`1541hud/CHANGELOG.md`](1541hud/CHANGELOG.md) for 1541HUD/DriveHUD release history.
- [`.github/workflows/release.yml`](.github/workflows/release.yml) for the tag-triggered GitHub release workflow.

Stable release rules:

- `v0.0.30` is the immutable hardware-proven DriveHUD baseline.
- `v0.0.31` is the current stable 1541HUD release and rename baseline.
- Published tags are never moved or rewritten.
- Generated BIN/UF2 files are build products and are not committed to the repository.
- Runtime changes require a new version; documentation-only cleanup does not.

The historical upstream OneROM release procedure can be found in the original project at [piersfinlayson/one-rom](https://github.com/piersfinlayson/one-rom).
