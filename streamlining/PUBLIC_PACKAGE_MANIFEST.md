# Intended public-package manifest

This is a selection manifest for the future public repository. It is not a
command to change this private repository's visibility and it does not alter
the qualified private release.

## Include

| Area | Reason |
| --- | --- |
| `streamlining/` | Upstream pin, overlay generator, ROM-free exporter, package validator, and public-boundary records. |
| `firmware-control` V8.1 core delta and active `1541HUD-USB-v0.8.1` / `1541HUD-Host-Control-v0.8.1` source | UB3 integration source. The core itself is recreated from the pinned upstream checkout and the generated overlay. |
| `firmware-monitor` V8.1 core delta, active Monitor plugins, and `shared/1541hud_mailbox.h` | UB4 passive-monitor integration source. The core itself is recreated from the pinned upstream checkout and the generated overlay. |
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
| UB4 compose inputs `1541-c000.325302-01.bin` and the retained Universal bootloader binary | Do not redistribute binary ROM material without an explicit license/provenance decision. A public template must require a user-provided input verified by size and hash. |
| `firmware-*/releases/`, historical EXEs, `outputs/`, `work/`, and build kits | Generated artifacts, old candidates, or third-party material not yet cleared for public redistribution. |
| `c64-onerom-configurator/` | Deferred until its Control-board discovery issue is separately repaired and qualified. |
| S-JiffyDOS files and tests | Experimental, unqualified, and explicitly outside the current product scope. |
| USB serial numbers, local paths, SSH helpers, and local diagnostics | Operator/machine-specific information. |

## Required user-supplied inputs in a public build

A public build can compile the two firmware bases and project plugins once the
documented toolchain/dependency prerequisites are supplied. It cannot compose
a drive image until the owner supplies lawful ROM inputs and creates a private
composition manifest. The public package defines a `private-roms/` directory
ignored by Git; it does not contain or fetch ROMs.

At minimum the templates need:

1. legal 8 KiB C/E pages for a standard Lower or Upper socket build;
2. legal 8 KiB C/A/E page triplet(s) only for the optional three-page build;
3. any bootloader binary for which redistributable source or permission is not
   available.

The public documentation must say which files are needed, where they are
expected, their exact 8 KiB size, and their private-build SHA-256 reference.
It must not provide download links to copyrighted ROMs or the bytes
themselves.

## Explicitly deferred decisions

- Whether the Universal bootloader may be redistributed as source or as a
  binary. The current private chain starts from a retained binary base whose
  original source/provenance is incomplete, so it stays out until resolved.
- License/provenance status of the DOSNAME D64/PRG utility before publication.
- C64 configurator repair and release.
- Additional public prebuilt firmware beyond the two named stock releases.
