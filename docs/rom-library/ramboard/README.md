# RAMBoard memory map and software cautions

## Original CLD RAMBoard

The original CLD design provides an 8 KiB RAM window at `$8000-$9FFF`. Its
decode and bus-address multiplexing circuitry are part of the hardware design;
OneROM does not generate the RAMBoard `/CS` signal. Verify the actual board
schematic, its chip-select wiring, and the fitted RAM before using software
that depends on this region.

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
