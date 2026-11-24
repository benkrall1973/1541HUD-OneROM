# Public package boundary

This document defines what belongs in the **public** 1541HUD / OneROM
Fire-24G repository. It is the current release boundary, not a plan for a
future repository and not a substitute for the build or wiring guides.

## Included and intentionally public

| Area | Reason |
| --- | --- |
| `roms/` and `roms/configs/` | Curated drive ROM/KERNAL assets, source references, RAMBoard docs, and ROM-free composition examples. JiffyDOS/S-JiffyDOS folders contain documentation and project-authored source only, without licensed payloads. See `roms/ASSETS.md` for provenance limits. |
| `utilities/rom-pages/` and `tools/maintenance/` | ROM page utility, validator, and public-boundary records. |
| `C64 Configurator/` | R20 V1.4.31 BASIC selector source, standard-library tokenizer, PowerShell build wrapper, tracked PRG, checksum manifest, and technical documentation. The PRG is the only newly permitted `.PRG` payload; no DOS ROM or C64 KERNAL payload is included. |
| `source/one-rom-v0.8.1` | Supported source derived from official `v0.8.1`, with checked-in host fixes, source provenance and unsupported GUI trees excluded. |
| `overlays/host-dependencies-v0.8.1.*` and host dependency helpers/tests | Historical checksummed host patch and helpers. Current exports verify the incorporated fixes; the original snapshot is preserved in history/private backups. |
| Active `1541HUD-USB-v0.8.1` and `1541HUD-Host-Control-v0.8.1` source | UB3 integration source. USB plugin 0.8.1.18 is the current source and supports the 10 selectable ROM slots used by the r21 image. The wrapper builds both plugins against the official pinned core in an ignored work tree. |
| `firmware-monitor` README, active Monitor plugins, `shared/1541hud_mailbox.h`, manifest, and two baseline Monitor inputs | UB4 passive-monitor integration source. The wrapper exports the pinned source into an isolated work directory and applies the passive-core overlay; the checked-in OneROM bootloader and stock C000 baseline input reproduce the public UB4 Monitor image without a hidden local kit. |
| `docs/BUILD_UB3_CONTROL.md` | Short beginner path to build the Control firmware; detailed composition and engineering references remain separate. |
| ROM-free private-input instructions and UB3 CLI composition examples | Explain the two standard C/E socket layouts and optional C/A/E decoder layout. The r21 no-JiffyDOS composition uses public, checksummed ROM pages and can be rebuilt from its checked-in manifest. |
| V0.8.0.58 HUD source, launcher, tests, build script, Windows x64 executable, provenance record, and third-party notices | The owner confirmed the packaged EXE starts and its menus are navigable. Drive-control testing of this exact packaged EXE remains pending. Apply Only requires Control USB 0.8.1.17 or newer; USB 0.8.1.18 is the current source. Apply Only restoration after a 1541-only power cycle requires UB3 to lose power; use a data-capable cable with host VBUS +5 V blocked, or power-cycle UB3 explicitly. |
| `docs/FIRE24G_V81_BUILD_AND_INSTALL.md`, `docs/FIRE24G_V81_ENGINEERING_DOSSIER.md`, and sanitized wiring/build documentation | Hardware integration, prerequisites, flash/recovery, GPIO, LED and acceptance contract. |
| Three named Fire-24G images under `releases/` | UB3 Control stock two-ROM V51 baseline, separate stock two-ROM V53 Apply Only candidate, and UB4 passive Monitor image; the validator permits only these exact binaries. The GitHub prerelease also attaches the r21 four-profile no-JiffyDOS candidate. Its composition manifest is checked in as `releases/r21-basic-no-jiffydos-compose.json`; the generated r21 binary remains a release asset and is not tracked in Git. |

## Explicit exclusions

| Area | Why it is excluded |
| --- | --- |
| `roms/private/` and arbitrary ROM payloads | `roms/private/` is ignored by Git and excluded from public packages. Only exact asset files listed in `roms/ASSETS.sha256` are permitted. |
| Any composed firmware `.bin` except the three named Fire-24G images | Custom OneROM firmware images must not be added. ROM-library inputs are not composed firmware images. |
| `firmware-*/releases/`, historical EXEs, `outputs/`, `work/`, and build kits | Generated artifacts, old candidates, or third-party material not yet cleared for public redistribution. |
| Any unlisted `.PRG` file, including custom ROM/configuration payloads | Only `C64 Configurator/releases/C64-OneROM-Configurator-r20-V1.4.31.PRG` is allowed by the package validator. |
| Licensed JiffyDOS ROMs and C64 KERNALs; S-JiffyDOS generated images/pages | JiffyDOS is commercial software. Public folders contain documentation and project-authored source only; builders supply licensed inputs. |
| USB serial numbers, local paths, SSH helpers, and local diagnostics | Operator/machine-specific information. |

The package validator at `tools/maintenance/Test-Public-Package.ps1` enforces
the structural portion of this boundary. It does not decide copyright status;
maintainers must still review proposed additions.

The pinned OneROM source also includes synthetic ROM-parser test fixtures under
`source/one-rom-v0.8.1/images/test/`. These are deterministic test data, not
Commodore DOS or C64 KERNAL images; the package validator permits only the
enumerated upstream fixtures there.

## Builder-supplied private inputs

A public build can compile the firmware bases and project plugins once the
documented toolchain prerequisites are supplied. The UB4 Monitor composition
is fully reproducible from its tracked baseline inputs. UB3 Control image
composition still requires ROM inputs and a private composition manifest. The
public package defines a `roms/private/` directory ignored by Git; JiffyDOS
inputs remain owner-supplied and are not fetched by the build tools.

At minimum the templates need:

1. legal 8 KiB C/E pages for a standard Lower or Upper socket build;
2. legal 8 KiB C/A/E page triplet(s) only for the optional three-page build;

The public documentation must say which files are needed, where they are
expected, their exact 8 KiB size, and their private-build SHA-256 reference.
Do not add JiffyDOS binaries or generated S-JiffyDOS outputs. The curated
stock, SpeedDOS+, DolphinDOS, and original CLD RAMBoard utility-disk files are
explicitly enumerated and hash-checked. The RAMBoard D64 is a utility disk, not
a drive ROM. See [notices](../NOTICE.md) and the
[asset list](../roms/ASSETS.sha256).

## Deferred scope

- Exact source/build provenance for the checked-in SpeedDOS+ profile.
- DOSNAME D64/PRG diagnostic utility is not included; its provenance and license must be cleared before adding it.
- Exact V1.4.31 C64 screen-interaction retest; the checked-in source and PRG are a reproducible public baseline, not a claim of newly completed hardware qualification.
- Additional prebuilt firmware binaries in the public Git tree beyond the named stock baseline, V53 candidate, and Monitor images. The separately attached r21 test candidate is a GitHub release asset and is not part of the tracked package; its source composition manifest is tracked.

