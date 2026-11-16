# ROM library asset provenance

`ASSETS.sha256` lists every binary file in this library with its byte count
check enforced by the package validator and its SHA-256 digest. Drive and
host pages are 8192 bytes unless noted.

## Stock DOS 2.6

The `stock-dos-2.6/` folder contains the extracted 1541 longboard and 1541-II
drive pages plus stock C64 KERNAL 901227-03. The A page mirrors E for each
drive revision. These are ROM dumps/pages; Commodore assembly source was not
available in the project archive. The root MIT license does not apply to
these contents.

## SpeedDOS+

The `speeddos-plus/` folder contains the project's tested SpeedDOS+ drive
profile pages and matching English C64 KERNAL. The A page mirrors E. The
project archive did not contain the exact build source or sufficient
provenance to reproduce these specific files. Do not substitute the separate
RAMBoard-II patch or infer compatibility from a matching label. The files are
included as the tested profile assets; no source-rebuild claim is made.

## DolphinDOS 2.0

The `dolphindos-2/source/` files and Unlicense are from the
[DolphinDOS 2 upstream repository](https://github.com/donnchawp/DolphinDOS2).
The C64 KERNAL assembly and upstream Makefile are included. That Makefile
builds only the 8 KiB C64 KERNAL. The drive `1541.asm` file is supplied as
upstream reference material, but the upstream instructions do not provide a
reproducible build recipe for the drive ROM. The 32 KiB `reference/dd2_1541.rom`
and the extracted C/A/E pages preserve the drive profile used by this project.
Do not mirror E into A: DolphinDOS has a distinct A page.

## JiffyDOS and S-JiffyDOS

The `jiffydos/` and `s-jiffydos/` folders hold documentation and patch/build
notes, plus project-authored OneROM autostart source. They do not contain
licensed JiffyDOS inputs, JiffyDOS C64 KERNALs, the S-JiffyDOS generator disk,
or generated JiffyDOS/S-JiffyDOS pages or images. Builders must obtain their
own licensed JiffyDOS copy and generator where required. Never add those
payloads to this public repository.
