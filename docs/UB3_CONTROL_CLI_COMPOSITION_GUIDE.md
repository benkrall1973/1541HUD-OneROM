# UB3 Control: CLI Composition Guide

## Scope

This is the supported public procedure for building a Fire-24G **UB3 Control**
image. It uses the OneROM command-line interface (CLI). The legacy GUI
configurator is not part of this procedure.

The public repository supplies documented stock baseline inputs/images, but
not the optional third-party DOS collections. For custom sets, provide only
ROM images you are entitled to use. Do not commit those
images or a completed private manifest to a public repository.

## Choose the hardware layout

The public package provides three CLI templates. Choose the one that matches
the installed hardware; these are not interchangeable.

| Template | Hardware | Required owner-supplied ROM pages | RAMBoard required |
| --- | --- | --- | --- |
| `UB3-Control-Lower-Socket.template.json` | Standard lower-socket Control installation | C and E | No |
| `UB3-Control-Upper-Socket.template.json` | Standard upper-socket Control installation | E and C | No |
| `control-compose.template.json` | Three-page Fire-24G C/A/E installation, including DolphinDOS-style sets | C, A and E | No; it requires the documented A-page decode wiring |

The standard Lower and Upper templates are the normal two-page options for a
user who has no RAMBoard or A-page wiring. The DolphinDOS/C/A/E template is
only for hardware that has the documented X1/Y5 A-page and X2/Y7 E-page decode
wiring.

For the three-page C/A/E installation, every selectable DOS profile is a
three-page set in this order:

1. `$C000-$DFFF` (C page)
2. `$A000-$BFFF` (A page, selected through X1 / 74LS138 `/Y5`)
3. `$E000-$FFFF` (E page, selected through X2 / 74LS138 `/Y7`)

Each file must be exactly 8192 bytes. The C/A/E order must not be rearranged.

## Prerequisites

- A clean clone of this project.
- OneROM CLI `onerom` on `PATH`, or an absolute path to it.
- ARM GNU Toolchain and GNU Make, as described in
  `FIRE24G_V81_BUILD_AND_INSTALL.md`.
- Built UB3 base firmware and the two Control plugins:
  - `1541HUD-USB` system plugin
  - `1541HUD-Host-Control` user plugin
- Your lawful C, A and E ROM page files.

## Prepare private inputs

Create a local, ignored folder:

```text
private-roms/
  control/
    slot0-c000-dfff.bin
    slot0-a000-bfff.bin
    slot0-e000-ffff.bin
  UB3-Control-DolphinDOS-CAE.json
```

For the C/A/E layout, copy
`templates/control-compose.template.json` to
`private-roms/UB3-Control-DolphinDOS-CAE.json`, then edit only its local labels
and ROM file paths. Add additional complete C/A/E groups locally for more DOS
profiles. Never publish that finished JSON file or the referenced ROM files.

For a standard two-page installation, copy the corresponding Lower or Upper
template instead and provide only its two 8192-byte files. Do not add a dummy
A page merely because another person's build uses one.

## Build and compose the image

For a fresh public checkout, the supported route is WSL2/Linux. The wrapper
builds the patched Control core and both Control plugins from this repository,
then invokes the locally built OneROM CLI to compose your private manifest:

```bash
bash consolidation/scripts/build-control-v0.8.1.sh \
  private-roms/UB3-Control-DolphinDOS-CAE.json \
  output/onerom-ub3-control-fire24g.bin
```

The bundled Windows `tools/onerom-v0.5.0.exe` is for inspecting and flashing
the public release images; it does not replace ARM GCC, Make, Cargo, and a
POSIX shell required for a fresh source build.

## Compose an already-built core

From the repository root, substitute your actual base firmware path and chosen
private manifest path:

```powershell
onerom firmware build `
  --config private-roms/UB3-Control-DolphinDOS-CAE.json `
  --board fire-24-g `
  --base-firmware firmware-control/.work/control-core-v0.8.1/firmware-control/source/control-baseline-v0.8.1-release/firmware/build/onerom-rp235x.bin `
  --output output/onerom-ub3-control-fire24g.bin `
  --force
```

The output file is a private, composed firmware image; keep it out of Git.

## Verify before flashing

- Confirm the CLI reports the Fire-24G board and the expected plugins.
- Confirm every selected ROM page is 8192 bytes.
- Confirm the C/A/E page order matches the qualified 74LS138 wiring.
- Compare the generated slot labels with the intended DOS profiles.
- Flash only one board at a time and verify USB enumeration before connecting
  it to the drive.

For wiring, GPIO roles, LED behavior, reset timing and qualified hardware
assumptions, use `FIRE24G_V81_ENGINEERING_DOSSIER.md` and
`FIRE24G-VERIFIED-WIRING.md`.
