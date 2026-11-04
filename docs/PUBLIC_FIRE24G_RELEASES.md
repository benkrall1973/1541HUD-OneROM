# Fire-24G public release images

These are the only prebuilt firmware images distributed with the public
package. Both target **Fire-24G** hardware only.

| Board | File | SHA-256 |
| --- | --- | --- |
| UB3 Control | `releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-test-v0.8.1.bin` | `2F07F383DFEE805CA944E2A57AF82C16D4C0D7F05C7990505264D04C382F7568` |
| UB4 Monitor | `releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.28.bin` | `1351A0C4448C9C442E053923841226B16A6351B271314CDCC56C0638D7BBD406` |

## UB3 Control stock test image

This image contains stock 1541 DOS 2.6, the OneROM Universal bootloader,
1541HUD Control USB and Host-Control plugins. It provides IEC 8/9, active-low
350 ms reset, and optional write-protect override/sense. It has no JiffyDOS,
SpeedDOS, DolphinDOS, RAMBoard or S-JiffyDOS profile.

Required wiring: SEL_A for IEC 8/9 and SEL_B through 1 kOhm for reset.
Optional wiring: SEL_C write-protect override transistor and SEL_D 22k/22k
write-protect sense divider.

## UB4 passive Monitor image

Flash this only to the passive Monitor board. It is not an interchangeable UB3
image. Its required capture wiring is SEL_A to UC2 pin 23, SEL_B to UC2 pin 17,
X1 to UC4 pin 34, and X2 to UC4 pin 39. It does not control write protect.

## Flashing

The repository bundles upstream OneROM CLI 0.5.0 at
`tools/onerom-v0.5.0.exe`. No separate CLI download is required for the
release-image workflow.

1. Close 1541HUD so it releases the board CDC ports.
2. Put only the intended board into BOOTSEL.
3. In PowerShell, run `./tools/onerom-v0.5.0.exe --unrecognised --board fire-24-g scan` and record the target serial.
4. Program with `./tools/onerom-v0.5.0.exe --serial <SERIAL> program --firmware ./releases/<IMAGE>.bin --verify`, then reboot normally and verify USB enumeration.
5. Test UB3 before installing UB4: IEC 8/9, reset, then an immediate directory
   read. Test UB4 only after UB3 is confirmed healthy.

These are stock test releases. Use the ROM-free CLI templates to compose
custom Lower/Upper or C/A/E images with ROM pages you are entitled to use.
