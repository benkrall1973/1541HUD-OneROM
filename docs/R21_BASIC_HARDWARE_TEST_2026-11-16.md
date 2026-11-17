# r21 Basic UB3 hardware test — 2026-11-16

The public **r21 Basic multi-ROM, no-JiffyDOS** image was flashed to a Fire-24G
UB3 Control board and tested in the drive by the project owner.

## Confirmed working

- Commodore DOS
- SpeedDOS
- DolphinDOS 2.0
- IEC address control
- Write-protect status and override

The image's default startup profile is DOS 2.6 Original, with IEC address 8.
The image uses the C/A/E three-page layout and requires the documented
74LS138 decoder wiring. The test used the existing 74LS138 setup; no wiring
change was reported.

This record reflects the checks reported by the owner. It does not claim that
every profile-specific C64 KERNAL, parallel transfer mode, or other hardware
combination was separately qualified.

## Image identity

- Release asset: `onerom-1541HUD-Control-UB3-fire-24-g-basic-no-jiffydos-r21.bin`
- Size: 917,504 bytes
- SHA-256: `4A8D2477B1C250111D846FBBF51CFFA5664A95E86627C28DEC8C0CF754827588`
- Release: [Fire-24G UB3 images](https://github.com/benkrall1973/1541HUD-OneROM/releases/tag/v8.1-fire24g-ub3-images-2026-11-16)
