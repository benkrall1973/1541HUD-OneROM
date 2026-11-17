# Fire-24G images and flashing

The [current GitHub release](https://github.com/benkrall1973/1541HUD-OneROM/releases/tag/v8.1-fire24g-ub3-images-2026-11-16)
contains two **UB3 Control** images. The UB4 Monitor image remains in the
repository as a separate file; it is not one of the current release downloads.
All images target OneROM **Fire-24G** hardware only.

## Current release downloads

| Image | Download name | Size | SHA-256 | Status |
| --- | --- | ---: | --- | --- |
| Stock DOS 2.6 two-ROM UB3 | `onerom-1541HUD-Control-UB3-fire-24-g-stock-2rom-v0.8.1.bin` | 262,144 bytes | `A066F70C1B9636714C0D0A0694D4B981641DF3E1DDD024D0E210097697BCBAFD` | Flashed and drive-tested |
| r21 Basic multi-ROM UB3, no JiffyDOS | `onerom-1541HUD-Control-UB3-fire-24-g-basic-no-jiffydos-r21.bin` | 917,504 bytes | `4A8D2477B1C250111D846FBBF51CFFA5664A95E86627C28DEC8C0CF754827588` | Flashed and owner-tested: Commodore DOS, SpeedDOS, DolphinDOS 2.0, parallel cable, IEC address control, and write-protect status/override; see [test record](R21_BASIC_HARDWARE_TEST_2026-11-16.md) |

The stock image has the OneROM bootloader and stock 1541 DOS 2.6. It was
tested in a 1541: DOS boot, directory read and game load, saving IEC address
and ROM selection, write-protect operation, reset, and Control/Monitor
communication. Use the stock C64 KERNAL.

The r21 image starts with **DOS 2.6 Original** (NV0 = 1) and IEC address **8**
(NV2 = 8). I downloaded the release asset and inspected it locally with the
repository's OneROM CLI. The image reports Fire-24G, USB plugin 0.8.1.18,
Host-Control 0.8.1.5, 11 total slots including the bootloader, and the four
visible profiles below. Slots 5–10 are marked `@hidden:Unused ROM slot`; no
JiffyDOS or S-JiffyDOS pages are present. The image contains 33 8-KiB drive
pages. Its embedded NV data starts at offset `0x2F000`, with NV0 set to slot 1
and NV2 to address 8; the NV-sector hash is
`431ACA2B89BF1051557C02F7F9A94DCD4CA6DA16AE4F68BD458E51104FBD3045`. The
image is 917,504 bytes and its SHA-256 matches the release entry above.

Its four visible drive profiles and C64 KERNAL pairings are:

All four profiles use the three-page **C/A/E layout**. The r21 image therefore
requires the documented **74LS138 decoder wiring**: X1 to `/Y5` (pin 10) and
X2 to `/Y7` (pin 7). It is not the image for a direct two-ROM X1 socket setup;
use the stock two-ROM image for that wiring.

| Drive profile | C64-side KERNAL / hardware |
| --- | --- |
| DOS 2.6 Original | Stock C64 KERNAL |
| DOS 2.6 1541-II | Stock C64 KERNAL; this profile was tested in a longboard 1541 |
| SpeedDOS | English SpeedDOS+ C64 KERNAL; parallel cable for its fast path |
| DolphinDOS 2.0 | DolphinDOS 2.0 C64 KERNAL, parallel cable, and compatible drive-side RAM for its fast path |

JiffyDOS and S-JiffyDOS drive ROMs, C64 KERNALs, and runtime helper are not
included in r21. The same four-profile manifest is available at
[`r21-basic-no-jiffydos-compose.json`](../releases/r21-basic-no-jiffydos-compose.json)
and uses public ROM assets. The owner reports successful drive tests for
Commodore DOS, SpeedDOS, DolphinDOS 2.0, parallel cable operation, IEC address
control, and write-protect status/override. The owner reports the parallel
cable had previously been tested and working; the DOS tests confirmed it was
working in this setup. See the [r21 hardware test record](R21_BASIC_HARDWARE_TEST_2026-11-16.md)
for the reported test scope.

## Other repository images

| Board | File | Size | SHA-256 | Notes |
| --- | --- | ---: | --- | --- |
| UB4 passive Monitor | `releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.32.bin` | 229,376 bytes | `475EB25252ADB9435BD7569297C6C16E72DD7830980B88DA9268EAEFC9BF43BD` | Flash only to the passive UB4 Monitor board; do not use on UB3. |
| UB3 V53 Apply Only candidate | `releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-2rom-apply-only-v0.8.1.17.bin` | 262,144 bytes | `4798C3DD88B9018581E5285E529F6C69A678308AFD25202CF761E1ACF6AA55B8` | Historical candidate in the repository; it is not attached to the current GitHub release. |

The recorded V53 candidate combines the same two stock 1541 profiles with
Control USB 0.8.1.17 and Host-Control 0.8.1.5. USB 0.8.1.17 adds `ROMIEC_NOW` for HUD
V0.8.0.53/V0.8.0.58 Apply Only. The live address change was verified on the
drive without changing the selected DOS profile or requiring a 1541 power
cycle. The checked-in binary was normalized after testing to remove local
diagnostic paths and was not reflashed. Do not use it with the older USB
0.8.1.16 baseline image. See the [candidate record](STOCK_CONTROL_APPLY_ONLY_CANDIDATE_2026-11-10.md).

The stock image attached to GitHub is the original drive-tested binary
(SHA-256 above). The checked-in copy at
`releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-2rom-v0.8.1.bin` has two
equal-length diagnostic-path strings normalized and a different hash
(`DB2A9F85E58A54341FB5C6572252069E4F8414717406A17DA294BB0E8A69B842`). That
normalized copy was not reflashed. The normalizer is
[`Sanitize-Control-Diagnostics.ps1`](../tools/maintenance/Sanitize-Control-Diagnostics.ps1).

The public stock composition uses the separately named 1541 E000 901229-06AA
page tracked in the ROM library and checksum manifest. It is not byte-identical
to the other 1541 and 1541-II E pages; do not substitute either based only on a
similar filename. See
[`roms/stock-dos-2.6/README.md`](../roms/stock-dos-2.6/README.md).

## Flashing

Close 1541HUD first so it releases the USB board. Connect only the board you
intend to program, put that board into BOOTSEL, and scan it:

```powershell
./tools/onerom-v0.6.0-hud1.exe --unrecognised scan --board fire-24-g
```

Check the serial number and board role. Program the intended image and verify
it (replace `<SERIAL>` and `<IMAGE.bin>`):

```powershell
./tools/onerom-v0.6.0-hud1.exe --serial <SERIAL> program `
  --firmware <PATH-TO-IMAGE.bin> --verify
```

For the r21 GitHub release asset, download the `.bin` first and use its local
path for `<IMAGE.bin>`; r21 is not checked into the repository. Reboot and
check USB enumeration. On UB3, test IEC 8/9, reset, write protect, ROM selection,
directory reads, and a game load. For r21, consult its test record for confirmed
profiles and controls, including the working parallel cable. Test UB4
only after UB3 is confirmed healthy, using the Monitor-specific wiring in the
[verified wiring guide](FIRE24G-VERIFIED-WIRING.md).

For V53 Apply Only, a 1541-only power cycle restores the saved address only if
UB3 also loses power. If USB data remains connected, use a data cable that
passes D+/D− and ground while blocking host VBUS +5 V, or power-cycle UB3
separately. See the [Apply Only candidate record](STOCK_CONTROL_APPLY_ONLY_CANDIDATE_2026-11-10.md).

Use the [ROM-free composition examples](../roms/configs/README.md) and
[ROM page preparation guide](THIRD_PARTY_DOS_PAGE_PREPARATION.md) to build
custom images from ROM inputs you are licensed to use. Keep licensed inputs
and custom images out of public Git.
