# DolphinDOS 2.0

DolphinDOS 2 is a separate system with its own 1541 drive ROM and C64-side
KERNAL. It does **not** use JiffyDOS as its base and does not need a JiffyDOS
patch. Use the standard DolphinDOS 2.0 firmware for the 1541 longboard; there
is no separate longboard DOS variant in this library. The original hardware
package combines the drive DOS ROM, an 8 KiB RAM expansion at `$8000`, a
parallel link, and a matching C64 KERNAL.

In the tested OneROM/RAMBoard arrangement, the 8 KiB RAM at `$8000-$9FFF`
serves as DolphinDOS's track buffer. The drive DOS occupies three 8 KiB ROM
pages: A (`$A000-$BFFF`), C (`$C000-$DFFF`), and E (`$E000-$FFFF`). These
three pages are the OneROM layout for the 1541, not three independent DOS
variants.

For the high-speed path, use the matching DolphinDOS C64 KERNAL, DolphinDOS
drive ROM, parallel cable, and the required drive-side RAM configuration.
With the ordinary IEC cable alone, do not call it a tested parallel-speed
setup.

## Included files and build source

This folder includes the upstream source and Unlicense, a separate reference
drive image, the hardware-tested OneROM drive pages, and the matching C64
KERNAL used in this project. The source files and license are from the
[DolphinDOS 2 upstream repository](https://github.com/donnchawp/DolphinDOS2).
The extracted pages and KERNAL are 8192 bytes each; sizes and SHA-256 values
are recorded in [the library asset manifest](../ASSETS.sha256).

Use the included `drive-pages/` files as the known-good C/A/E inputs for this
project. They are the same pages from the user's working DolphinDOS setup;
this documentation update does not alter them. Do not mirror E into A.

The file `reference/dd2_1541.rom` is a different 32 KiB community/emulator
image. Its four consecutive 8 KiB chunks do **not** match the three
hardware-tested pages. Do not run the generic page splitter on that reference
image and substitute the output. The upstream repository's `1541.asm` is
reference disassembly, not a reproducible drive-ROM build recipe. Its
Makefile builds the C64 KERNAL only.

If you intentionally want to create a new drive-page set, start with the
[original project's drive-side firmware archive](https://e4aws.silverdr.com/resources/dolphindos2/2dosa_c.bin.zip)
and its [hardware description](https://e4aws.silverdr.com/projects/dolphindos2/).
Verify the archive's exact image size, source revision, and address map first;
only then prepare the `$A000-$FFFF` C/A/E pages for OneROM. The `$8000-$9FFF`
region is the RAM buffer in the DolphinDOS hardware configuration, not a
fourth ROM page. Do not replace the tested pages until a newly composed image
has been tested on the target longboard and RAMBoard wiring. Retain the
included Unlicense when reusing upstream source. Parallel-transfer behavior
still depends on the matching host KERNAL, cable, and compatible drive-side
hardware.
