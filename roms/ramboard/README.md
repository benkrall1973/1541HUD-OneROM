# RAMBoard memory map and software cautions

## Original CLD reference files

The [`original-cld/` folder](original-cld/) preserves the original CLD material
provided for this project:

| File | Contents |
|---|---|
| [`RAMBOard-1541-021.pdf`](original-cld/RAMBOard-1541-021.pdf) | Original schematic and PCB layout pages for CLD RAMBoard revision 021. |
| [`RAMBOard-1541-Installation-1.1.pdf`](original-cld/RAMBOard-1541-Installation-1.1.pdf) | Original installation and utility-disk manual. It describes an invasive installation that removes the 1541 CPU and installs a socket; treat it as historical instructions and verify the exact drive/board revision before following it. |
| [`ramboard.d64`](original-cld/ramboard.d64) | 35-track utility disk image, including `RAMBOARD TEST`. It is a utility disk, not a ROM image. |
| [`RAMBOard-1541-021top_Layout2.png`](original-cld/RAMBOard-1541-021top_Layout2.png), [`RAMBOard-1541-021brd_Layout1.png`](original-cld/RAMBOard-1541-021brd_Layout1.png), [`RAMBOard-1541-021bot_Layout3.png`](original-cld/RAMBOard-1541-021bot_Layout3.png) | Separate top, board, and bottom layout images supplied with the schematic. |

The original documents identify CLD copyright. They are kept with their
original filenames and attribution; this repository's MIT license does not
relicense them.

The manual describes the test command as `LOAD"RAMBOARD TEST",8,1` followed
by `RUN`. Run it only with a compatible original CLD RAMBoard and a known-safe
DOS profile. **Do not run RAMBoard utilities while the S-JiffyDOS shadow
runtime is active:** software that writes to `$8000-$9FFF` can overwrite that
runtime and corrupt the active DOS.

## Original CLD RAMBoard

The original CLD design provides an 8 KiB RAM window at `$8000-$9FFF`. Its
decode and bus-address multiplexing circuitry are part of the hardware design;
OneROM does not generate the RAMBoard `/CS` signal. Verify the actual board
schematic, its chip-select wiring, and the fitted RAM before using software
that depends on this region.

The original [CLD RAMBoard-1541-021 schematic and board-trace drawing](https://web.archive.org/web/20240522013822/http://d81.de/CLD-RAMBOard/RAMBOard-41.shtml)
shows the board's 74LS139 RAM decode and 74F157 address multiplexing. That RAM
decode is on the RAMBoard; it is separate from the 74LS138 used to select the
OneROM C/A/E pages. This reference applies to the original CLD board only. Do
not substitute RAMBoard ][ wiring or assume the same connections on a 1541-II
revision.

The tested S-JiffyDOS shadow build uses that RAM window to hold executable
S-JiffyDOS code. **Writes to `$8000-$9FFF` while that profile is active can
overwrite the running code and corrupt the DOS.** See
[S-JiffyDOS 1541](../s-jiffydos/README.md).

## Do not conflate board revisions

The [RAMBoard ][ project](https://github.com/ytmytm/1541-RAMBOardII) follows
the original design but changes the memory/ROM implementation and documents
1541-II installation details. Its source is useful research, but matching the
`$8000-$9FFF` address range alone does not prove electrical, firmware, or
longboard compatibility. The public OneROM project has not qualified its
JiffyDOS cache patch on the original CLD board.
