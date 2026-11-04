# Streamlining: Fire-24G V8.1 integration package

## Objective

This directory is the staging area for a clean, reproducible integration
package based on OneROM `releases/0.8.1` commit
`379584110988b407064ddf1d18582dab5df1da4a`.

The final package will be the source of a **new public repository with fresh
history**. It must reproduce the project-owned Fire-24G integration without
shipping third-party or proprietary DOS ROM payloads. The existing private
repository remains the engineering archive and is not to be made public.

This branch is the source for that export, but is still the private engineering
archive. Create the clean tree with `scripts/Export-Public-Fire24G-V81.ps1`.
It uses an allowlist and calls `Test-Public-Package.ps1` before success.

## Why the overlay has two source targets

UB3 Control and UB4 Monitor both start from the same pinned upstream OneROM
release, but they intentionally modify different parts of that source. UB3
needs the control/NV-write/reset integration; UB4 needs the passive PIO/DMA
and Monitor capture integration. A patch applied to one upstream checkout
cannot represent both configurations safely.

The eventual single generated overlay will therefore target a prepared project
layout containing two clean upstream checkouts:

| Checkout | Role | Resulting project path |
| --- | --- | --- |
| Control checkout | UB3, state-changing ROM/control service | `firmware-control/source/control-baseline-v0.8.1-release/` |
| Monitor checkout | UB4, electrically passive monitor | `firmware-monitor/source/monitor-baseline-v0.8.1-upstream/` |

The overlay is strictly the upstream-core/project-plugin delta. The export
script adds build scripts, HUD source/tests, wiring documentation and ROM-free
private-input instructions. The overlay is a generated review artifact, not a
second hand-maintained implementation.

## Canonical inputs on this branch

| Item | Canonical location |
| --- | --- |
| Upstream pin | `upstream/releases/0.8.1` at `379584110988b407064ddf1d18582dab5df1da4a` |
| UB3 core | `firmware-control/source/control-baseline-v0.8.1-release/` |
| UB3 USB plugin | `firmware-control/active-plugins/system/1541HUD-USB-v0.8.1/` |
| UB3 Host-Control plugin | `firmware-control/active-plugins/user/1541HUD-Host-Control-v0.8.1/` |
| UB4 core | `firmware-monitor/source/monitor-baseline-v0.8.1-upstream/` |
| UB4 USB plugin | `firmware-monitor/active-plugins/system/1541HUD-Monitor-USB/` |
| UB4 Monitor plugin | `firmware-monitor/active-plugins/user/1541HUD-Monitor/` |
| UB3 build entry point | `consolidation/scripts/build-control-v0.8.1.sh` |
| UB4 build entry point | `firmware-monitor/Build-Monitor-Fire24G-V0.8.1.ps1` |
| HUD source | `1541hud/source/1541HUD_V0.8.0.51.py` |
| Current wiring and installation record | `docs/FIRE24G_V81_BUILD_AND_INSTALL.md` |
| Engineering contract: GPIO, LED, USB and timing rules | `docs/FIRE24G_V81_ENGINEERING_DOSSIER.md` |
| UB3 ROM-free CLI composition guide | `docs/UB3_CONTROL_CLI_COMPOSITION_GUIDE.md` |
| ROM-free clean-package exporter | `streamlining/scripts/Export-Public-Fire24G-V81.ps1` |
| Clean-package validator | `streamlining/scripts/Test-Public-Package.ps1` |

The C64 configurator is deliberately out of scope for this streamlining pass.
It remains private work in progress until its UB3 discovery regression is
separately repaired and qualified.

## Public-release boundary

Do not copy any file from these areas into the new public repository:

- `roms/` binary payloads, including JiffyDOS, DolphinDOS, SpeedDOS, stock
  Commodore ROM dumps, or derived page images;
- composed UB3 images containing one or more ROM payloads;
- S-JiffyDOS experiment source, ROMs, images, or tests;
- historical Windows executables and private build archives;
- USB serial numbers, operator-specific paths, SSH material, or local build
  products.

The public package may include schemas and templates that name
user-supplied ROM files, plus a size/hash verification procedure. It must not
include the ROM bytes themselves.

## Work sequence

1. Inventory canonical inputs and identify stale/historical material.
2. Create a ROM-free project layout and template manifests.
3. Generate one overlay from the pinned clean upstream trees to that layout.
4. Write a bootstrap script that fetches/checks out the exact upstream commit,
   applies the overlay, and validates the expected file set.
5. Rebuild UB3 and UB4 from the bootstrap layout, then run the existing
   hardware acceptance checklist before promoting anything.
6. Create a new public repository from the clean package only. Do not change
   visibility of this private repository.

## Non-goals for this branch

- rewriting or deleting historical private-repository history;
- repairing the C64 configurator;
- changing qualified firmware behavior;
- publishing a ROM-bearing binary image.
