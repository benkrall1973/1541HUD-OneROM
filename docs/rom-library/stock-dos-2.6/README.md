# Commodore DOS 2.6

## Inputs and host ROM

The longboard 1541 and 1541-II DOS images are distinct source variants, but
the user confirmed the project's DOS 2.6 1541-II OneROM profile passed on a
longboard 1541 in the r18 image test. This confirms that specific profile on
that drive; do not assume every 1541-II ROM build is interchangeable. For
ordinary IEC operation, pair either drive DOS with the stock C64 KERNAL.

## OneROM packaging

Use the two-page C/E template for a standard two-ROM setup. Use the C/A/E
template only when the documented A-page decode wiring is installed. Do not
manufacture or mirror a third page unless the source documentation says that
page is a mirror.

## Included files

This folder contains the profile-specific extracted pages for the original
1541 and 1541-II DOS 2.6 images, plus the stock C64 KERNAL used in testing.
Each page is 8192 bytes. `A000-BFFF.bin` and `E000-FFFF.bin` are the same page
for these profiles; `C000-DFFF.bin` differs by drive revision. Use the two-page
C/E layout for standard OneROM wiring unless the documented A-page decode is
actually installed.

File hashes and sizes are recorded in [the library asset manifest](../ASSETS.sha256).
These are drive/host inputs, not precomposed OneROM firmware images.

See [ROM page preparation](../../THIRD_PARTY_DOS_PAGE_PREPARATION.md) and the
[CLI composition guide](../../UB3_CONTROL_CLI_COMPOSITION_GUIDE.md).
