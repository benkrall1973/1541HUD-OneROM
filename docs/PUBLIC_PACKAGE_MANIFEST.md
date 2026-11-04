# Intended public-package manifest

This is a selection manifest for the future public repository. It is not a
command to change this private repository's visibility and it does not alter
the qualified private release.

## Include

| Area | Reason |
| --- | --- |
| `templates/`, `tools/maintenance/`, and this manifest | ROM-free templates, package validator, and public-boundary records. |
| `source/one-rom-v0.8.1` | Single pinned, unreleased upstream V8.1 development snapshot used by both roles, including the exact vendored UB4 build dependencies `tinyusb`, `picobootx`, and `pico-sdkless`. |
| `firmware-control/overlays/control-core-v0.8.1.patch` and active `1541HUD-USB-v0.8.1` / `1541HUD-Host-Control-v0.8.1` source | UB3 integration source. The wrapper applies the tracked Control-core delta to the pinned source in an ignored work tree. |
| `firmware-monitor` active Monitor plugins, `shared/1541hud_mailbox.h`, manifest, and two baseline Monitor inputs | UB4 passive-monitor integration source. The wrapper builds directly from the pinned source with the passive-core flag; the checked-in OneROM bootloader and stock C000 baseline input reproduce the public UB4 Monitor image without a hidden local kit. |
| ROM-free private-input instructions and the UB3 CLI composition guide | Explain the two standard C/E socket layouts and the optional C/A/E DolphinDOS-style layout without distributing ROM bytes; users create a private composition manifest locally. |
| `1541hud/source/1541HUD_V0.8.0.51.py`, launcher, tests, and current HUD documentation | The qualified desktop operator tool. |
| `docs/FIRE24G_V81_BUILD_AND_INSTALL.md`, `docs/FIRE24G_V81_ENGINEERING_DOSSIER.md`, and sanitized wiring/build documentation | Hardware integration, prerequisites, flash/recovery, GPIO, LED and acceptance contract. |
| `utilities/dosname-rom-probe/` source and documentation | Optional diagnostic utility, after its provenance is checked. |
| Two named stock Fire-24G release images under `releases/` | Ready-to-flash UB3 Control stock-test and UB4 passive Monitor images; the validator permits only these exact binaries. |

## Exclude

| Area | Why it is excluded |
| --- | --- |
| `roms/` payloads and any ROM-derived page image | Third-party/proprietary or unverified redistribution status. |
| Any composed `.bin` except the two named stock Fire-24G release images | Custom or third-party DOS payloads must not be distributed. |
| `firmware-*/releases/`, historical EXEs, `outputs/`, `work/`, and build kits | Generated artifacts, old candidates, or third-party material not yet cleared for public redistribution. |
| `c64-onerom-configurator/` | Deferred until its Control-board discovery issue is separately repaired and qualified. |
| S-JiffyDOS files and tests | Experimental, unqualified, and explicitly outside the current product scope. |
| USB serial numbers, local paths, SSH helpers, and local diagnostics | Operator/machine-specific information. |

## Required user-supplied inputs in a public build

A public build can compile the firmware bases and project plugins once the
documented toolchain prerequisites are supplied. The UB4 Monitor composition
is fully reproducible from its tracked baseline inputs. UB3 Control image
composition still requires owner-supplied lawful ROM inputs and a private
composition manifest. The public package defines a `private-roms/` directory
ignored by Git; it does not contain or fetch third-party DOS ROMs.

At minimum the templates need:

1. legal 8 KiB C/E pages for a standard Lower or Upper socket build;
2. legal 8 KiB C/A/E page triplet(s) only for the optional three-page build;

The public documentation must say which files are needed, where they are
expected, their exact 8 KiB size, and their private-build SHA-256 reference.
It must not provide download links to copyrighted ROMs or the bytes
themselves.

## Explicitly deferred decisions

- License/provenance status of the DOSNAME D64/PRG utility before publication.
- C64 configurator repair and release.
- Additional public prebuilt firmware beyond the two named stock releases.
