# Public package boundary

This document defines what belongs in the **public** 1541HUD / OneROM
Fire-24G repository. It is the current release boundary, not a plan for a
future repository and not a substitute for the build or wiring guides.

## Included and intentionally public

| Area | Reason |
| --- | --- |
| `templates/`, `private-roms/` templates, `utilities/rom-pages/`, and `tools/maintenance/` | Templates, utilities, validator, and public-boundary records. |
| `docs/rom-library/` | Stock DOS and C64 KERNAL, tested SpeedDOS+ factory inputs/pages/KERNAL, DolphinDOS 2 source/license/reference/pages/KERNAL, plus JiffyDOS/S-JiffyDOS documentation and project-authored patch/build source without licensed JiffyDOS payloads. See `ASSETS.md` for provenance limits. |
| `source/one-rom-v0.8.1` | Supported source derived from official `v0.8.1`, with checked-in host fixes, source provenance and unsupported GUI trees excluded. |
| `overlays/host-dependencies-v0.8.1.*` and host dependency helpers/tests | Historical checksummed host patch and helpers. Current exports verify the incorporated fixes; the original snapshot is preserved in history/private backups. |
| Active `1541HUD-USB-v0.8.1` and `1541HUD-Host-Control-v0.8.1` source | UB3 integration source. The wrapper builds these custom plugins against the official pinned core in an ignored work tree. |
| `firmware-monitor` active Monitor plugins, `shared/1541hud_mailbox.h`, manifest, and two baseline Monitor inputs | UB4 passive-monitor integration source. The wrapper exports the pinned source into an isolated work directory and applies the passive-core overlay; the checked-in OneROM bootloader and stock C000 baseline input reproduce the public UB4 Monitor image without a hidden local kit. |
| ROM-free private-input instructions and the UB3 CLI composition guide | Explain the two standard C/E socket layouts and the optional C/A/E DolphinDOS-style layout without distributing ROM bytes; users create a private composition manifest locally. |
| V0.8.0.51 HUD baseline and V0.8.0.53 Apply Only candidate, launchers, tests, and HUD documentation | V0.8.0.53 uses runtime-only IEC address selection and requires UB3 USB plugin 0.8.1.17. Apply Only restoration after a 1541-only power cycle requires UB3 to lose power; use a data-capable cable with host VBUS +5 V blocked, or power-cycle UB3 explicitly. |
| `docs/FIRE24G_V81_BUILD_AND_INSTALL.md`, `docs/FIRE24G_V81_ENGINEERING_DOSSIER.md`, and sanitized wiring/build documentation | Hardware integration, prerequisites, flash/recovery, GPIO, LED and acceptance contract. |
| `utilities/dosname-rom-probe/` source and documentation | Optional diagnostic utility, after its provenance is checked. |
| Three named Fire-24G images under `releases/` | UB3 Control stock two-ROM V51 baseline, separate stock two-ROM V53 Apply Only candidate, and UB4 passive Monitor image; the validator permits only these exact binaries. The V53 candidate is also attached to the current GitHub Release. |

## Explicit exclusions

| Area | Why it is excluded |
| --- | --- |
| `roms/` and arbitrary ROM payloads | Only exact files under `docs/rom-library/stock-dos-2.6/`, `speeddos-plus/`, and `dolphindos-2/` are included; the validator hash-checks this allowlist. |
| Any composed firmware `.bin` except the three named Fire-24G images | Custom OneROM firmware images must not be added. ROM-library inputs are not composed firmware images. |
| `firmware-*/releases/`, historical EXEs, `outputs/`, `work/`, and build kits | Generated artifacts, old candidates, or third-party material not yet cleared for public redistribution. |
| `c64-onerom-configurator/` | Deferred until its Control-board discovery issue is separately repaired and qualified. |
| Licensed JiffyDOS ROMs and C64 KERNALs; S-JiffyDOS generated images/pages | JiffyDOS is commercial software. Public folders contain documentation and project-authored source only; builders supply licensed inputs. |
| USB serial numbers, local paths, SSH helpers, and local diagnostics | Operator/machine-specific information. |

The package validator at `tools/maintenance/Test-Public-Package.ps1` enforces
the structural portion of this boundary. It does not decide copyright status;
maintainers must still review proposed additions.

## Builder-supplied private inputs

A public build can compile the firmware bases and project plugins once the
documented toolchain prerequisites are supplied. The UB4 Monitor composition
is fully reproducible from its tracked baseline inputs. UB3 Control image
composition still requires ROM inputs and a private composition manifest. The
public package defines a `private-roms/` directory ignored by Git; JiffyDOS
inputs remain owner-supplied and are not fetched by the build tools.

At minimum the templates need:

1. legal 8 KiB C/E pages for a standard Lower or Upper socket build;
2. legal 8 KiB C/A/E page triplet(s) only for the optional three-page build;

The public documentation must say which files are needed, where they are
expected, their exact 8 KiB size, and their private-build SHA-256 reference.
Do not add JiffyDOS binaries or generated S-JiffyDOS outputs. The curated
stock, SpeedDOS+, and DolphinDOS files are explicitly enumerated and
hash-checked. See [notices](../NOTICE.md) and the
[asset list](rom-library/ASSETS.sha256).

## Deferred scope

- Exact source/build provenance for the checked-in SpeedDOS+ profile.
- License/provenance status of the DOSNAME D64/PRG utility before publication.
- C64 configurator repair and release.
- Additional public prebuilt firmware beyond the named stock baseline, V53 candidate, and Monitor images.

