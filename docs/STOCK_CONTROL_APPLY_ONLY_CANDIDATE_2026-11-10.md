# Stock Control V53 Apply Only candidate

This document records the public Fire-24G two-ROM candidate that pairs stock
1541 DOS 2.6 pages with Control USB plugin 0.8.1.17 for HUD V0.8.0.53.

## Artifact

- Image: `releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-2rom-apply-only-v0.8.1.17.bin`
- Size: 262,144 bytes
- SHA-256: `4798C3DD88B9018581E5285E529F6C69A678308AFD25202CF761E1ACF6AA55B8`
- Core: official OneROM v0.8.1, Fire-24G
- Plugins: USB 0.8.1.17 and Host-Control 0.8.1.5
- ROM profiles: stock C000 with the OneROM Universal E000 bootloader; stock
  C000 plus stock DOS 2.6 E000 (`901229-06AA`)

## Build and inspection

The reproducible configuration is
[`releases/stock-2rom-apply-only-compose.json`](../releases/stock-2rom-apply-only-compose.json).
It uses the same public stock ROM pages as the established USB 0.8.1.16
baseline. USB 0.8.1.17 adds `ROMIEC_NOW`, which changes SEL_A at runtime
without writing NV2 and is required by HUD V0.8.0.53 Apply Only.

Offline inspection confirmed the Fire-24G board type, both plugin versions,
two ROM profiles, and the expected page labels. The exact public candidate
V0.8.0.53 Apply Only behavior was also user-verified on the drive: the IEC
address changed live, the selected DOS ROM remained working, and the 1541 did
not need a power cycle for the live change. Two equal-length TinyUSB diagnostic
filename strings were normalized with
[`Sanitize-Control-Diagnostics.ps1`](../tools/maintenance/Sanitize-Control-Diagnostics.ps1)
to remove a local build path after the drive test; the image size and executable
bytes are unchanged. The normalized public artifact itself was not reflashed
after that diagnostic-string-only edit. Keep the USB 0.8.1.16 image as the
established V51 baseline; this .17 candidate pairs with V53.

## Power-cycle behavior

Apply Only changes the live IEC address without changing the selected DOS ROM,
writing NV2, or requiring a 1541 power cycle. If you want to verify that a
1541-only power cycle restores the saved NV2 address while USB remains connected,
UB3 must also lose power and reboot. Use a data-capable cable or adapter that
blocks host VBUS +5 V while passing D+/D− and ground, or disconnect USB/power-cycle
UB3 explicitly. Automatic 1541 power-off sensing is not implemented.

## Test boundary

The V51 baseline's drive test is recorded in
[`STOCK_CONTROL_DRIVE_TEST_2026-11-10.md`](STOCK_CONTROL_DRIVE_TEST_2026-11-10.md).
The V53 Apply Only behavior was separately user-verified as described above.
The public image was not reflashed after its equal-length diagnostic path
strings were normalized; the firmware and executable bytes were unchanged.
Separately, the project USB plugin source later corrected the invalid-address
diagnostic from `MAX=11` to `MAX=9`. The published .16 and .17 images were not
rebuilt for this source-only correction and may still display `MAX=11`; they
continue to accept only IEC 8/9.
