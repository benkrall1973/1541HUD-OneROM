# Fire-24G public release images

These are the only prebuilt firmware images distributed with the public
package. Both target **Fire-24G** hardware only.

| Board | File | SHA-256 |
| --- | --- | --- |
| UB3 Control | `releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-test-v0.8.1.bin` | `734736EB8E2E990FFD101A3918E8834DBAA10F8173A1B876DDAD4AC75E378D40` |
| UB4 Monitor | `releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.32.bin` | `475EB25252ADB9435BD7569297C6C16E72DD7830980B88DA9268EAEFC9BF43BD` |

See [the baseline runbook](V81_BASELINE_RUNBOOK.md) for component versions and test scope.
The stock Control image was recomposed with the drive-tested components; this
exact stock composition has not itself been flashed. Monitor code was tested
in the drive with device NV preserved; the public image has default NV and
two sanitized diagnostic filename strings. Instructions and addresses are unchanged.

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

The repository bundles OneROM CLI 0.6.0 with the 1541HUD host-dependency overlay
at `tools/onerom-v0.6.0-hud1.exe`. Its build manifest and dependency license
notices accompany it. No separate CLI download is required. The CLI can
compose, inspect and flash images; source firmware builds still need the
separate documented compiler toolchain.

1. Close 1541HUD so it releases the board CDC ports.
2. Put only the intended board into BOOTSEL.
3. In PowerShell, run `./tools/onerom-v0.6.0-hud1.exe --unrecognised scan --board fire-24-g` and record the target serial.
4. Program with `./tools/onerom-v0.6.0-hud1.exe --serial <SERIAL> program --firmware ./releases/<IMAGE>.bin --verify`, then reboot normally and verify USB enumeration.
5. Test UB3 before installing UB4: IEC 8/9, reset, then an immediate directory
   read. Test UB4 only after UB3 is confirmed healthy.

These are stock test releases. Use the ROM-free CLI templates to compose
custom Lower/Upper or C/A/E images with ROM pages you are entitled to use.
