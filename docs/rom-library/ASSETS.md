# ROM library asset provenance

`ASSETS.sha256` lists every ROM library binary with its SHA-256 digest. The
package validator checks the recorded hashes and expected byte counts. Drive
and host pages are 8192 bytes unless noted.

## Stock DOS 2.6

The `stock-dos-2.6/` folder contains the extracted 1541 longboard and 1541-II
drive pages plus stock C64 KERNAL 901227-03. The A page mirrors E for each
drive revision. These are ROM dumps/pages; Commodore assembly source was not
available in the project archive. The root MIT license does not apply to
these contents.

## SpeedDOS+

The `speeddos-plus/` folder contains the original files supplied in
`mr_z_-_speeddos+_roms.zip`, retaining their factory filenames in
`source-images/`: `1541lower`, `1541upper_sd`, `1541both_sd`, and
`kernal_en_sd`. The archive has prebuilt ROM images, not assembler source.
The project maintainer confirmed that these SpeedDOS files may be published;
the supplied archive has no separate license file, so this records that
permission source without implying the repository's MIT license covers them.
`1541both_sd` is exactly 16 KiB: its first 8 KiB matches `1541lower`, and its
second 8 KiB matches `1541upper_sd`. For this profile, the A page is an exact
copy of the upper/E page. The processed OneROM pages and C64 KERNAL are kept
alongside these original inputs. These are the tested SpeedDOS+ profile
assets; do not substitute the unrelated RAMBoard-II patch.

## DolphinDOS 2.0

The `dolphindos-2/source/` files and Unlicense are from the
[DolphinDOS 2 upstream repository](https://github.com/donnchawp/DolphinDOS2).
The C64 KERNAL assembly and upstream Makefile are included. That Makefile
builds only the 8 KiB C64 KERNAL. The drive `1541.asm` file is supplied as
upstream reference material, but is a disassembly rather than a reproducible
drive-ROM build source. The original project's page provides a separate
[drive-side firmware archive](https://e4aws.silverdr.com/resources/dolphindos2/2dosa_c.bin.zip).
The included C/A/E pages are the user's hardware-tested working set. The
32 KiB `reference/dd2_1541.rom` is a separate community/emulator reference;
its 8 KiB chunks do not match the tested pages, so it is not their source and
must not be naively split to replace them. DolphinDOS has a distinct A page;
do not mirror E into A.

## JiffyDOS and S-JiffyDOS

The `jiffydos/` and `s-jiffydos/` folders hold documentation and patch/build
notes, plus project-authored OneROM autostart source. They do not contain
licensed JiffyDOS inputs, JiffyDOS C64 KERNALs, the S-JiffyDOS generator disk,
or generated JiffyDOS/S-JiffyDOS pages or images. Builders must obtain their
own licensed JiffyDOS copy and generator where required. Never add those
payloads to this public repository.
