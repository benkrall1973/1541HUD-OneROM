# SpeedDOS+

SpeedDOS+ is a separate DOS family, not a JiffyDOS patch. Keep its drive ROM,
C64-side KERNAL, track-count variant, and RAMBoard/cache variant paired as a
single documented build set.

The high-speed transfer path requires the matching SpeedDOS+ C64 KERNAL and
parallel cable. With IEC only, the drive may still operate, but that does not
exercise the parallel fast path. The original host KERNAL is included as
`source-images/kernal_en_sd`.

## VIA compatibility test

On a standard longboard 1541 (250446 PCB, Alps mechanism) using the matching
SpeedDOS+ 2.7 35-track drive ROM and C64 KERNAL with Sven's parallel adapter,
Bruce Lee loaded in about 30 seconds with the original 6522 VIA installed.
The WDC `W65C22N6TPG-14` did not work in that same SpeedDOS setup; replacing it
with the original 6522 restored successful parallel loading. Use the original
6522 for this tested configuration rather than the W65C22N6TPG-14. This is a
setup-specific hardware result, not a universal claim about every W65C22N or
every 1541 adapter. Because the chip swap also reseats the adapter connection,
the result implicates the replacement VIA or its contact but does not isolate
the electrical mechanism.

## Build the OneROM page set

The four original, prebuilt files from the supplied SpeedDOS+ archive are
preserved with their factory names in `source-images/`:

- `1541both_sd` — 16 KiB drive image, C page followed by E page.
- `1541lower` — 8 KiB C page, byte-identical to the first half of
  `1541both_sd`.
- `1541upper_sd` — 8 KiB upper/E page, byte-identical to the second half.
- `kernal_en_sd` — matching 8 KiB English C64 KERNAL.

The current tested OneROM set needs three 8 KiB pages in C/A/E order. For this
SpeedDOS image, A mirrors E. From the repository root, run:

```powershell
.\utilities\rom-pages\Split-1541-RomPages.ps1 `
  -InputPath .\roms\speeddos-plus\source-images\1541both_sd `
  -OutputDirectory .\output\speeddos-plus `
  -PageOrder CE -MirrorEToA -Prefix SpeedDOS
```

This writes the C and E pages from `1541both_sd` and makes an exact copy of E
for A. It is page preparation only; use the matching C/A/E composition
template to build a OneROM image. Alternatively, use the already included
`drive-pages/` files, whose hashes match these source files. The `source-images`
are prebuilt ROMs, not assembler source; this repository does not rebuild the
SpeedDOS program itself.

## Included profile

The `drive-pages/` files are the tested SpeedDOS+ profile used by this
project. The A page mirrors E. The matching English SpeedDOS+ C64 KERNAL is in
`host-kernel/`. Each page and KERNAL is 8192 bytes; hashes are in [the library
asset manifest](../ASSETS.sha256).

These are profile inputs, not a composed OneROM image. The exact assembler
source for the SpeedDOS program is not present, so users can prepare and
compose the supplied image but cannot rebuild SpeedDOS from its original
source here. The
[RAMBoardII repository](https://github.com/ytmytm/1541-RAMBOardII) is a
reference for related SpeedDOS builds, but its RAMBoard-II patches are not the
source of this original-longboard profile. This is the tested profile; it does
not include the unqualified RAMBoard/cache variant.
