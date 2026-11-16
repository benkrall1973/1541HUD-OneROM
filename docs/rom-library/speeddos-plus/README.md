# SpeedDOS+

SpeedDOS+ is a separate DOS family, not a JiffyDOS patch. Keep its drive ROM,
C64-side KERNAL, track-count variant, and RAMBoard/cache variant paired as a
single documented build set.

The high-speed transfer path requires the matching SpeedDOS+ C64 KERNAL and
parallel cable. With IEC only, the drive may still operate, but that does not
exercise the parallel fast path. The private project has a C64-side
`kernal_en_sd.bin`; it is not a public ROM asset.

Before publishing a build recipe, record the exact source release, 35/40-track
choice, C64 KERNAL source, parallel-cable pinout, and whether the drive build
uses the RAMBoard. Do not mix a 35-track host ROM with a 40-track drive image.

## Included profile

The `drive-pages/` files are the tested SpeedDOS+ 35-track profile used by
this project. The A page mirrors E in this profile. The matching English
SpeedDOS+ C64 KERNAL is in `host-kernel/`. Each file is 8192 bytes; hashes are
in [the library asset manifest](../ASSETS.sha256).

These are profile inputs, not a composed OneROM image. The exact source/build
recipe for these files was not present in the project archive, so do not claim
that this profile can be rebuilt from source here. The
[RAMBoardII repository](https://github.com/ytmytm/1541-RAMBOardII) is a
reference for related SpeedDOS builds, but its RAMBoard-II patches are not the
source of this original-longboard profile. This is the tested profile; it does
not include the unqualified RAMBoard/cache variant.
