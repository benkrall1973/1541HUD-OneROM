# 1541 OneROM DOS and KERNAL library guide

This catalog separates the **drive DOS ROM** from the **C64-side KERNAL** and
records the profile inputs and build notes used by this project. It includes
the stock DOS, SpeedDOS+, and DolphinDOS 2.0 files listed below. The JiffyDOS
and S-JiffyDOS folders contain documentation and project-authored patch/source
material only: they do not contain licensed JiffyDOS ROMs, generated pages,
or completed JiffyDOS images.

## Start here

1. Identify the exact drive and hardware: 1541 longboard, 1541C/1541-II,
   original CLD RAMBoard, RAMBoard ][, and whether the parallel cable is fitted.
2. Pick the DOS profile and read its folder below before splitting or patching
   anything. The three-page OneROM profile is ordered C, A, E; each page is
   exactly 8 KiB. That layout is a OneROM packaging requirement, not proof that
   an arbitrary 24/32 KiB input is arranged correctly.
3. Keep each DOS's drive pages, build recipe, host KERNAL notes, and test record
   together. Never treat a profile label as proof of provenance.
4. Build with the matching composition template in `templates/`. Keep licensed
   JiffyDOS inputs and all private manifests/output images in ignored local
   `private-roms/` / `output/` paths.

## Known profiles and host-side pairing

| Profile | What it is | C64-side ROM / requirement | Build status |
|---|---|---|---|
| Commodore DOS 2.6 | Stock 1541 drive DOS; the longboard and 1541-II pages are distinct. | Stock C64 KERNAL. | Drive pages and stock C64 KERNAL are included in `stock-dos-2.6/`. |
| JiffyDOS | Commercial serial fast-DOS pair. | Licensed JiffyDOS C64 KERNAL for JiffyDOS fast serial transfers; stock KERNAL remains ordinary IEC. | Official drive ROM is an owner-supplied input. See `jiffydos/`. |
| S-JiffyDOS 1541 | A drive-side patch derived from JiffyDOS 1541; independent of S-JiffyDOS 64. | No special S-JiffyDOS C64 KERNAL is required. A JiffyDOS C64 KERNAL can use its serial fast path. | The private project has a generated and hardware-tested build; public recipe is limited by input/tool redistribution rights. See `s-jiffydos/`. |
| DolphinDOS 2.0 | Separate drive DOS and C64 KERNAL project; it is not built from JiffyDOS. | Matching DolphinDOS C64 KERNAL, parallel cable, and required drive-side RAM support for its fast parallel path. | Source, license, reference image, extracted pages, and host KERNAL are included. See `dolphindos-2/`. |
| SpeedDOS+ | Separate drive DOS and C64 KERNAL pair. | Matching SpeedDOS+ C64 KERNAL and parallel cable for the high-speed path; ordinary IEC alone is not the parallel path. | The tested profile's original images and C/A/E-ready pages are included. Use the page-splitting instructions in `speeddos-plus/`; assembler source is not available. |

## Drive ROM versus C64 KERNAL

Fast loading is a protocol shared by two machines. The **1541 drive ROM**
implements the drive side; the **C64 KERNAL** initiates or speaks the host
side. A drive ROM name by itself does not tell you which C64 KERNAL to use.

- Stock DOS pairs with the stock C64 KERNAL for ordinary IEC.
- JiffyDOS and S-JiffyDOS use the serial bus. A JiffyDOS C64 KERNAL enables
  JiffyDOS host-side fast transfers; S-JiffyDOS 1541 does not require a
  separate S-JiffyDOS 64 ROM.
- DolphinDOS 2 and SpeedDOS use separate parallel-drive systems. Their fast
  paths need the matching host ROM and physical parallel connection.

## Rights and sources

JiffyDOS is licensed commercial software. Obtain it through the [official
JiffyDOS site](https://www.jiffydos.com/), which lists authorized sales
channels and hosts its manuals. Link to the official manual page instead of
copying the manual into this repository. Do not put licensed inputs, derived
ROM pages, hashes of private payloads, or completed manifests in public Git.

The [DolphinDOS 2 source repository](https://github.com/donnchawp/DolphinDOS2)
contains separate drive and C64-side source/build instructions. The
[RAMBoard ][ project](https://github.com/ytmytm/1541-RAMBOardII) documents a
JiffyDOS RAM-cache patch, but its board and base-ROM assumptions must be
checked against the original CLD RAMBoard and the longboard before anyone uses
that patch here. It is a research reference, not yet this project's verified
build recipe.

## Folder index

- [Stock DOS 2.6](stock-dos-2.6/README.md)
- [JiffyDOS](jiffydos/README.md)
- [S-JiffyDOS 1541](s-jiffydos/README.md)
- [DolphinDOS 2.0](dolphindos-2/README.md)
- [SpeedDOS+](speeddos-plus/README.md)
- [RAMBoard memory map and S-JiffyDOS warning](ramboard/README.md)
- [Asset provenance notes](ASSETS.md)
- [Asset manifest and checksums](ASSETS.sha256)

