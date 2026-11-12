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
binary has not been separately drive-qualified. Two equal-length TinyUSB
diagnostic filename strings were normalized with
[`Sanitize-Control-Diagnostics.ps1`](../tools/maintenance/Sanitize-Control-Diagnostics.ps1)
to remove a local build path; the image size and executable bytes are unchanged.
Keep the USB 0.8.1.16 image as
the established V51 baseline, and use this .17 candidate when validating V53.

## Power-cycle behavior

Apply Only temporarily changes the live IEC address and leaves NV2 unchanged.
If the 1541 is power-cycled while USB VBUS continues to power UB3, the temporary
address remains until UB3 reboots. To keep USB data connected and make a drive
power cycle also shut down UB3, use a data-capable cable or adapter that blocks
host VBUS +5 V while passing D+/D− and ground. Alternatively, power-cycle UB3
explicitly. Automatic 1541 power-off sensing is not implemented.

## Test boundary

The V51 baseline's drive test is recorded in
[`STOCK_CONTROL_DRIVE_TEST_2026-11-10.md`](STOCK_CONTROL_DRIVE_TEST_2026-11-10.md).
That record does not qualify this .17 candidate image or the V53 Apply Only
power-cycle workflow.
