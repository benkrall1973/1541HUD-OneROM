# ROM library asset provenance

`ASSETS.sha256` lists every binary ROM-library asset and utility-disk image
with its SHA-256 digest. The package validator checks the recorded hashes and
expected byte counts. Drive and host ROM pages are 8192 bytes unless noted;
the RAMBoard D64 is a 174,848-byte, 35-track utility disk, not ROM data.

## Stock DOS 2.6

The `stock-dos-2.6/` folder contains the extracted 1541 longboard and 1541-II
drive pages plus stock C64 KERNAL 901227-03. The A page mirrors E for each
drive revision. The folder also contains E000 page 901229-06AA, fetched from
[Zimmers](http://www.zimmers.net/anonftp/pub/cbm/firmware/drives/new/1541/1541-e000.901229-06AA.bin)
and used by the stock two-ROM build. These are ROM dumps/pages; Commodore assembly source
was not available in the project archive. The root MIT license does not apply
to these contents.

## SpeedDOS+

The `speeddos-plus/` folder contains the original files supplied in
`mr_z_-_speeddos+_roms.zip`, retaining their factory filenames in
`source-images/`: `1541lower`, `1541upper_sd`, `1541both_sd`, and
`kernal_en_sd`. The archive has prebuilt ROM images, not assembler source.
The [CSDb archive record](https://csdb.dk/release/?id=21767&show=summary)
describes the supplied ROM files but gives no license or redistribution grant.
It is an archive listing, not rights-holder authorization. The project
maintainer confirmed separately that these SpeedDOS files may be published;
that permission is not independently documented in this repository. No
explicit SpeedDOS license was found in this online check (2026-11-19). The
repository's MIT license does not cover these files.
`1541both_sd` is exactly 16 KiB: its first 8 KiB matches `1541lower`, and its
second 8 KiB matches `1541upper_sd`. For this profile, the A page is an exact
copy of the upper/E page. The processed OneROM pages and C64 KERNAL are kept
alongside these original inputs. These are the tested SpeedDOS+ profile
assets; do not substitute the unrelated RAMBoard-II patch.

## DolphinDOS 2.0

The `dolphindos-2/source/` files and the upstream repository's
[Unlicense](https://github.com/donnchawp/DolphinDOS2/blob/master/LICENSE) are
from the [unofficial DolphinDOS2 repository](https://github.com/donnchawp/DolphinDOS2).
Its README says it is not affiliated with the original developers and that
version 1 retains the original DolphinDOS ROM files. The Unlicense is the
license published by that repository, but I found no separate grant from the
original DolphinDOS rights holders; the authority to relicense original ROM
content is therefore not established by the available online material.
The C64 KERNAL assembly and upstream Makefile are included. That Makefile
builds only the 8 KiB C64 KERNAL. The drive `1541.asm` file is supplied as
upstream reference material, but is a disassembly rather than a reproducible
drive-ROM build source. The original project's page provides a separate
[drive-side firmware archive](https://e4aws.silverdr.com/resources/dolphindos2/2dosa_c.bin.zip).
The included C/A/E pages are the user's hardware-tested working set. The
32 KiB `reference/dd2_1541.rom` is a separate community/emulator reference;
its exact source and redistribution terms are not established here, and its
8 KiB chunks do not match the tested pages. Do not treat the upstream
repository's Unlicense as covering this separate file without confirming its
origin, and do not naively split it to replace the tested pages. DolphinDOS
has a distinct A page; do not mirror E into A.

## JiffyDOS and S-JiffyDOS

The `jiffydos/` and `s-jiffydos/` folders hold documentation and patch/build
notes, plus project-authored OneROM autostart source. They do not contain
licensed JiffyDOS inputs, JiffyDOS C64 KERNALs, the S-JiffyDOS generator disk,
or generated JiffyDOS/S-JiffyDOS pages or images. Builders must obtain their
own licensed JiffyDOS copy and generator where required. Never add those
payloads to this public repository.

## Original CLD RAMBoard

`ramboard/original-cld/` contains the original schematic, installation manual,
three board-layout images, and the RAMBoard utility D64 supplied for this
project. The manual identifies CLD copyright and describes a revision-specific
installation; the files are preserved as references and are not relicensed by
the repository. See [the RAMBoard guide](ramboard/README.md) for the board
scope, utility-disk command, and S-JiffyDOS memory warning.
