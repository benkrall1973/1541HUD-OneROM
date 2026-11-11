# Fire-24G public release images

These are the prebuilt images distributed with the public package. Both target
**Fire-24G** hardware only.

| Board | File | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| UB3 Control | `releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-2rom-v0.8.1.bin` | 262144 | `A066F70C1B9636714C0D0A0694D4B981641DF3E1DDD024D0E210097697BCBAFD` |
| UB4 Monitor | `releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.32.bin` | 229376 | `475EB25252ADB9435BD7569297C6C16E72DD7830980B88DA9268EAEFC9BF43BD` |

The UB3 image has two selectable slots. Slot 0 combines the stock C000 page
with the OneROM Universal E000 bootloader; slot 1 combines the stock C000 page
with stock 1541 DOS 2.6 E000 (`901229-06AA`). It includes Control USB 0.8.1.16
and Host-Control 0.8.1.5. The public composition recipe is
[`releases/stock-2rom-compose.json`](../releases/stock-2rom-compose.json).
Its standard DOS page is fetched from the public OneROM/Zimmers source during
composition; no private ROM collection or device-specific NV is included.

The exact UB3 image was flashed and tested in a 1541. The user confirmed
standard DOS boot, a game load, successful saving of IEC address and ROM
settings, working write protection and reset, and correct Monitor and Control
operation on the drive. The 47 loose-board checks are recorded separately in
[`STOCK_CONTROL_LOOSE_TEST_2026-11-10.md`](STOCK_CONTROL_LOOSE_TEST_2026-11-10.md).
Full USB disconnect/reconnect persistence was not reported for this image.
The test record is
[`STOCK_CONTROL_DRIVE_TEST_2026-11-10.md`](STOCK_CONTROL_DRIVE_TEST_2026-11-10.md).

The UB3 image provides IEC 8/9, 350 ms active-low reset, and optional
write-protect override/sense. It has no JiffyDOS, SpeedDOS, DolphinDOS,
RAMBoard, or S-JiffyDOS profile. Required wiring is SEL_A for IEC 8/9 and
SEL_B through 1 kOhm for reset. Optional wiring is the documented SEL_C
write-protect override transistor and SEL_D 22k/22k write-protect sense
divider. Its known cosmetic invalid-address reply can display `MAX=11`,
although only IEC addresses 8 and 9 are accepted.

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
