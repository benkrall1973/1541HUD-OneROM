# Validation matrix and support boundary

This table distinguishes what this repository documents as verified from what
remains a builder responsibility. "Verified" means tested in the documented
Fire-24G / 1541 long-board installation. Other board revisions, cables, ROM
sets and modifications require their own qualification.

| Area | Status | Boundary |
| --- | --- | --- |
| Fire-24G UB3 Control role | Verified configuration | Use the documented long-board wiring, reset resistor, and documented USB-powered operation. |
| Exact public stock two-ROM Control image | Drive test passed with stated scope | User confirmed DOS boot, game load, saved IEC/ROM settings, write protection, reset, and Monitor/Control operation; full USB disconnect/reconnect persistence was not reported. See STOCK_CONTROL_DRIVE_TEST_2026-11-10.md. Earlier image's 47 loose-board checks remain recorded separately. |
| Windows CLI 0.6.0-hud1 | Loose-board exercised | Discovery, flash backup/read, programming with verification, inspection and USB-powered reboot passed on the tested Windows host. |
| Fire-24G UB4 passive Monitor role | Verified configuration | Passive capture only; UB4 does not serve ROM or control write protect. |
| IEC device 8/9 | Verified configuration | SEL_A wiring required; Apply+Save tested with OneROM continuously USB-powered. |
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

