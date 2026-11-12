# Fire-24G public images

These are the prebuilt images distributed with the public package. All target
**Fire-24G** hardware only.

| Board | File | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| UB3 Control baseline (HUD V51) | `releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-2rom-v0.8.1.bin` | 262144 | `DB2A9F85E58A54341FB5C6572252069E4F8414717406A17DA294BB0E8A69B842` |
| UB3 Control V53 Apply Only candidate | `releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-2rom-apply-only-v0.8.1.17.bin` | 262144 | `4798C3DD88B9018581E5285E529F6C69A678308AFD25202CF761E1ACF6AA55B8` |
| UB4 Monitor | `releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.32.bin` | 229376 | `475EB25252ADB9435BD7569297C6C16E72DD7830980B88DA9268EAEFC9BF43BD` |

The UB3 image has two selectable slots. Slot 0 combines the stock C000 page
with the OneROM Universal E000 bootloader; slot 1 combines the stock C000 page
with stock 1541 DOS 2.6 E000 (`901229-06AA`). It includes Control USB 0.8.1.16
and Host-Control 0.8.1.5. The public composition recipe is
[`releases/stock-2rom-compose.json`](../releases/stock-2rom-compose.json).
Its standard DOS page is fetched from the public OneROM/Zimmers source during
composition; no private ROM collection or device-specific NV is included.
Before publication, both UB3 binaries had their two equal-length TinyUSB
diagnostic filename strings normalized to remove a local build path. Firmware
size and executable bytes are unchanged by that string-only edit.
The repeatable byte-preserving normalizer is
[`Sanitize-Control-Diagnostics.ps1`](../tools/maintenance/Sanitize-Control-Diagnostics.ps1).

The exact UB3 V51 baseline image was flashed and tested in a 1541. The user confirmed
standard DOS boot, a game load, successful saving of IEC address and ROM
settings, working write protection and reset, and correct Monitor and Control
operation on the drive. The 47 loose-board checks are recorded separately in
[`STOCK_CONTROL_LOOSE_TEST_2026-11-10.md`](STOCK_CONTROL_LOOSE_TEST_2026-11-10.md).
Full USB disconnect/reconnect persistence was not reported for this image.
The test record is
[`STOCK_CONTROL_DRIVE_TEST_2026-11-10.md`](STOCK_CONTROL_DRIVE_TEST_2026-11-10.md).

The separate V53 candidate uses the same two stock 1541 profiles with Control
USB 0.8.1.17 and Host-Control 0.8.1.5. The USB plugin adds `ROMIEC_NOW`, which
V53 uses for runtime Apply Only without writing the saved NV2 address. Compose
it from [`stock-2rom-apply-only-compose.json`](../releases/stock-2rom-apply-only-compose.json).
The user verified V53 Apply Only on the drive: the live IEC address changed,
the selected DOS ROM stayed working, and no 1541 power cycle was needed for the
live change. The public binary later received only equal-length diagnostic-path
string normalization and was not reflashed afterward. See the
[candidate record](STOCK_CONTROL_APPLY_ONLY_CANDIDATE_2026-11-10.md). Do not use
V53 Apply Only with the older USB 0.8.1.16 baseline image.

The UB3 image provides IEC 8/9, 350 ms active-low reset, and optional
write-protect override/sense. It has no JiffyDOS, SpeedDOS, DolphinDOS,
RAMBoard, or S-JiffyDOS profile. Required wiring is SEL_A for IEC 8/9 and
SEL_B through 1 kOhm for reset. Optional wiring is the documented SEL_C
write-protect override transistor and SEL_D 22k/22k write-protect sense
divider. The checked-in .16 and .17 binaries may display `MAX=11` in an
invalid-address reply, although they accept only IEC addresses 8 and 9. The
project USB plugin source now reports `MAX=9`; the release images were not
rebuilt for that source-only diagnostic correction.

The HUD and current OneROM Control plugin expose only the wired IEC 8/9
selection. Commodore's [1541 User's Guide](https://s3.amazonaws.com/com.c64os.resources/weblog/sd2iecdocumentation/manuals/1541_Users_Guide.pdf)
documents a separate temporary device-number change from the C64 using DOS
`M-W`. That manual DOS operation is outside HUD/OneROM address control and has
not been qualified here for alternate DOS images.

For V0.8.0.53 Apply Only with Control USB plugin 0.8.1.17, use a
data-capable cable with host VBUS +5 V blocked if a 1541-only power cycle must
also restart UB3 and reload the saved NV2 address while USB data remains
connected. The cable must pass D+/D− and ground. A normal powered USB cable can
keep UB3 running and retain the temporary address; fully power-cycling UB3 also
reloads NV2. The V51 stock baseline remains on plugin 0.8.1.16; V53 uses the
separate .17 candidate listed above.

## UB4 passive Monitor image

Flash this only to the passive Monitor board. It is not interchangeable with
the UB3 image. Its required capture wiring is SEL_A to UC2 pin 23, SEL_B to
UC2 pin 17, X1 to UC4 pin 34, and X2 to UC4 pin 39. It does not control write
protect. Monitor firmware is 0.8.1.32 and Monitor USB is 0.8.1.5.

## Flashing

The repository bundles OneROM CLI 0.6.0 with the 1541HUD host-dependency
overlay at `tools/onerom-v0.6.0-hud1.exe`. Its build manifest and dependency
license notices accompany it. Close 1541HUD before flashing so it releases
the board USB ports.

1. Put only the intended board into BOOTSEL.
2. Run `./tools/onerom-v0.6.0-hud1.exe --unrecognised scan --board fire-24-g`
   and record the target serial.
3. Program with `./tools/onerom-v0.6.0-hud1.exe --serial <SERIAL> program --firmware ./releases/<IMAGE>.bin --verify`.
4. Reboot normally and verify USB enumeration. Test UB3 first: IEC 8/9,
   reset, write protect, ROM selection, then a directory read and game load.
   Test UB4 only after UB3 is confirmed healthy.

Use the ROM-free CLI templates to compose custom Lower/Upper or C/A/E images
from ROM pages you are entitled to use.
