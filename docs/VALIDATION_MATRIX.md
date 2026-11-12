# Validation matrix and support boundary

This table distinguishes what this repository documents as verified from what
remains a builder responsibility. "Verified" means tested in the documented
Fire-24G / 1541 long-board installation. Other board revisions, cables, ROM
sets and modifications require their own qualification.

| Area | Status | Boundary |
| --- | --- | --- |
| Fire-24G UB3 Control role | Verified configuration | Use documented long-board wiring and reset resistor. For Apply Only restoration after drive-only power cycle, block USB VBUS +5 V while passing data and ground, or power-cycle UB3. |
| Exact public stock two-ROM Control image | Drive test passed with stated scope | User confirmed DOS boot, game load, saved IEC/ROM settings, write protection, reset, and Monitor/Control operation; full USB disconnect/reconnect persistence was not reported. See STOCK_CONTROL_DRIVE_TEST_2026-11-10.md. Earlier image's 47 loose-board checks remain recorded separately. |
| Public two-ROM V53 Apply Only candidate (USB 0.8.1.17) | V53 Apply Only behavior user-verified on drive; normalized public binary not reflashed after diagnostic-string-only cleanup | User confirmed live IEC address change with the DOS ROM still working and no 1541 power cycle required. The public candidate uses stock ROM pages and Host-Control 0.8.1.5 with USB plugin .17, which implements `ROMIEC_NOW`. Its binary was not rebuilt for the later source-only `MAX=9` diagnostic correction and may still show `MAX=11`; both images accept only IEC 8/9. Keep the tested .16 image as the V51 baseline. See STOCK_CONTROL_APPLY_ONLY_CANDIDATE_2026-11-10.md. |
| Windows CLI 0.6.0-hud1 | Loose-board exercised | Discovery, flash backup/read, programming with verification, inspection and USB-powered reboot passed on the tested Windows host. |
| Fire-24G UB4 passive Monitor role | Verified configuration | Passive capture only; UB4 does not serve ROM or control write protect. |
| IEC device 8/9 | Verified configuration | SEL_A wiring required. Apply+Save was tested with USB-powered OneROM. Apply Only changes runtime SEL_A without NV2 writes; drive-only power cycle does not restore saved address if USB VBUS keeps UB3 powered. |
| Active-low reset | Verified configuration | SEL_B through 1 kOhm to C46-positive/CR7-anode reset node. |
| Write-protect sense/override | Verified configuration | Optional circuit; transistor and divider are mandatory where described. |
| 1541HUD Windows operation | Supported | Assign roles by USB serial; close HUD before flashing. |
| UB4 telemetry | Verified configuration | Requires all four documented passive capture wires. |
| Standard two-page C/E layouts | Supported build path | Builder supplies lawful 8 KiB ROM pages and matching socket wiring. |
| Three-page C/A/E layout | Supported build path | Requires documented 74LS138 decode wiring and builder-supplied pages. |
| SpeedDOS, JiffyDOS, DolphinDOS 2.0 layouts | Builder-supplied/private | No ROM payloads are distributed; hardware and legal input responsibility remain with the builder. |
| C64 configurator | Deferred | Not part of this public release. |
| S-JiffyDOS | Out of scope | Experimental material is deliberately excluded. |
| Other 1541 board revisions | Not qualified | Require independent electrical review and acceptance testing. |

## Build evidence

The repository contains the pinned source, project overlays, documented build
wrappers, and release-image hashes. The qualified source-build route is
WSL2/Linux. A derivative maintainer must record their own clean-host toolchain
and build result before presenting a derivative as reproducible.

