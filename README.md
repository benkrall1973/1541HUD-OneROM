# 1541HUD / OneROM Fire-24G

1541HUD adds USB control and optional drive monitoring to a OneROM Fire-24G
installation in a Commodore 1541. Use the **UB3 Control** board to serve drive
ROMs and control the IEC address, reset, and optional write protect. The
optional **UB4 Monitor** board passively observes drive activity.

## Drive ROMs you can use

The public [r21 Basic image](https://github.com/benkrall1973/1541HUD-OneROM/releases/tag/v8.1-fire24g-ub3-images-2026-11-16)
contains four selectable drive ROM profiles. Choose the matching C64 KERNAL
for fast DOS features; a drive ROM and a C64 KERNAL are separate things.

| Drive profile | Matching C64 KERNAL | Extra hardware / notes |
| --- | --- | --- |
| **DOS 2.6 Original** | Stock Commodore KERNAL | Standard IEC. Default startup profile. |
| **DOS 2.6 1541-II** | Stock Commodore KERNAL | Standard IEC; this profile was tested in a longboard 1541. |
| **SpeedDOS** | English SpeedDOS+ KERNAL | Parallel cable for fast transfer. IEC works with the parallel fast path disabled. |
| **DolphinDOS 2.0** | DolphinDOS 2.0 KERNAL | Parallel cable and compatible drive-side RAM for its fast path. |

The r21 image starts with **DOS 2.6 Original** and IEC address **8**. It omits
JiffyDOS and S-JiffyDOS. It passed software composition checks but has **not
yet been flashed or tested in a drive**; treat it as a test candidate.

The separate [stock two-ROM image](https://github.com/benkrall1973/1541HUD-OneROM/releases/tag/v8.1-fire24g-ub3-images-2026-11-16)
is the hardware-tested starting point. It contains stock 1541 DOS 2.6 and the
OneROM bootloader.

### JiffyDOS and S-JiffyDOS

You can make private images from ROMs and tools you are licensed to use. The
public repository includes preparation notes and project-authored material,
but **does not include licensed JiffyDOS ROMs or a ready-made JiffyDOS image**.
JiffyDOS fast serial transfers need the matching licensed JiffyDOS C64 KERNAL.
S-JiffyDOS uses RAMBoard shadow RAM at `$8000–$9FFF`; software that writes to
that range while S-JiffyDOS is active can corrupt its running code. See the
[JiffyDOS](roms/jiffydos/README.md), [S-JiffyDOS](roms/s-jiffydos/README.md),
and [RAMBoard](roms/ramboard/README.md) notes.

Browse the [ROM and KERNAL library](roms/README.md) for included pages, source,
build notes, and checksums. C64 KERNALs are installed on the C64; the UB3
images above contain drive-side ROM profiles, not a C64 KERNAL replacement.

## Get started

1. Read the [baseline runbook](docs/V81_BASELINE_RUNBOOK.md) and
   [verified wiring guide](docs/FIRE24G-VERIFIED-WIRING.md). These images are
   for Fire-24G boards; UB3 Control and UB4 Monitor images are not interchangeable.
2. Close 1541HUD, connect only the board you intend to program, enter BOOTSEL,
   and scan for its serial:

   ```powershell
   ./tools/onerom-v0.6.0-hud1.exe --unrecognised scan --board fire-24-g
   ```

3. Confirm the serial, download the selected `.bin` from the
   [release page](https://github.com/benkrall1973/1541HUD-OneROM/releases/tag/v8.1-fire24g-ub3-images-2026-11-16),
   then flash it with `program --verify`:

   ```powershell
   ./tools/onerom-v0.6.0-hud1.exe --serial <SERIAL> program `
     --firmware <PATH-TO-IMAGE.bin> --verify
   ```

   Compare the image's SHA-256 with the value shown on the release page.
4. Install and run the Windows HUD by following
   [1541HUD setup](1541hud/README.md). HUD V0.8.0.58 supports the newer ROM
   list and Apply Only flow; Apply Only needs the matching UB3 USB 0.8.1.17
   image. For a USB-connected power cycle to reload the saved address, UB3
   must lose power too: use a data cable that passes D+/D− and ground while
   blocking host +5 V, or power-cycle UB3 separately.

## Build or customize

- To build the Control firmware, follow [Build UB3 Control](docs/BUILD_UB3_CONTROL.md).
- To compose a custom drive image, start with the [ROM library](roms/README.md),
  [page preparation guide](docs/THIRD_PARTY_DOS_PAGE_PREPARATION.md), and
  [composition guide](docs/UB3_CONTROL_CLI_COMPOSITION_GUIDE.md). Examples
  are in `roms/configs/`.
- Keep private ROM inputs and custom images out of public Git. JiffyDOS is
  commercial software; use only ROMs and tools you are licensed to use.

## Repository map

| Folder | Contents |
| --- | --- |
| `releases/` | UB3 and UB4 firmware images and composition manifests |
| `1541hud/` | Windows HUD source and launch/build files |
| `firmware-control/` | UB3 Control firmware and plugins |
| `firmware-monitor/` | UB4 passive Monitor firmware and plugins |
| `roms/` | DOS ROM pages, KERNALs, references, and ROM-free config examples |
| `docs/` | Wiring, build, flashing, and verification guides |
| `source/` | Supported OneROM source snapshot and provenance |

The supported base is the official OneROM v0.8.1 release. See the
[source provenance](source/one-rom-v0.8.1/1541HUD-SOURCE.json),
[contribution guide](CONTRIBUTING.md), and [safety notes](SECURITY.md).

Disconnect power before changing wiring. Do not connect 5 V drive signals
directly to 3.3 V OneROM GPIO. Verify the board role and serial before flashing.
