# UB3 ROM layouts and configuration reference

For the beginner step-by-step build procedure, start with
[Build the UB3 Control firmware](BUILD_UB3_CONTROL.md). This page is the
reference for choosing a template and preparing custom ROM sets.

The public repository includes selected stock DOS 2.6, SpeedDOS+, and
DolphinDOS 2.0 inputs. The JiffyDOS folders contain build notes and patches,
not licensed JiffyDOS files or generated images. Supply only ROMs you are
entitled to use, and keep licensed images and completed private configurations
out of public Git repositories.

## Choose the template that matches your wiring

| Template | Physical setup | Pages in each ROM set |
| --- | --- | --- |
| `UB3-Control-Lower-Socket.template.json` | OneROM in lower `$C000` socket; direct X1 connection to the other socket | C and E |
| `UB3-Control-Upper-Socket.template.json` | OneROM in upper `$E000` socket; direct X1 connection to the other socket | E and C |
| `control-compose.template.json` | Three-page C/A/E setup with the documented 74LS138 decode wiring | C, A and E |

Use a two-page template when the drive has no 74LS138. Use the C/A/E template
only when the decoder wiring is installed. See the
[wiring guide](FIRE24G-VERIFIED-WIRING.md) before choosing.

## C/A/E page order and size

Each C/A/E ROM set has three 8 KiB pages, in this exact order:

1. C page: `$C000-$DFFF`
2. A page: `$A000-$BFFF`, selected by X1 / 74LS138 `/Y5`
3. E page: `$E000-$FFFF`, selected by X2 / 74LS138 `/Y7`

Do not reorder the files or substitute a full 16 KiB image for one 8 KiB page.
Split and inspect ROM pages with
[`Split-1541-RomPages.ps1`](../utilities/rom-pages/Split-1541-RomPages.ps1).

## Private custom configuration

Create a local folder named `private-roms/` at the repository root. Copy the
matching template there, then update its labels and file paths. Example:

```text
private-roms/
  my-drive.json
  control/
    slot0-c000-dfff.bin
    slot0-a000-bfff.bin
    slot0-e000-ffff.bin
```

For a two-page setup, provide only the two pages named by its template. For
multiple ROM profiles, add a complete group of pages for each profile. Keep
the JSON and ROM files local; `.gitignore` excludes the private inputs.

## Advanced: compose with an existing core and CLI

Most users should use the full build script in the
[build guide](BUILD_UB3_CONTROL.md). If you already have the built Control core
and a compatible `onerom` CLI, compose an image from the repository root:

```powershell
onerom firmware build `
  --config private-roms/my-drive.json `
  --board fire-24-g `
  --base-firmware firmware-control/.work/control-core-v0.8.1/one-rom-v0.8.1/firmware/build/onerom-rp235x.bin `
  --output output/onerom-ub3-control-fire24g.bin `
  --force
```

The output is a composed firmware image. Keep private builds and licensed ROM
inputs out of Git.
