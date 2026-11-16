# UB4 passive Monitor

The UB4 Monitor passively captures 1541 activity for 1541HUD. It does not
serve ROMs, reset the drive, or control write protection. Keep its image,
plugins, and wiring separate from UB3 Control.

## Current public image

- Monitor user plugin: **0.8.1.32**
- Monitor USB system plugin: **0.8.1.5**
- Fire-24G UB4 image: [`releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.32.bin`](../releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.32.bin)
- Image size and SHA-256: [`releases/v8.1-manifest.json`](../releases/v8.1-manifest.json)
- User-confirmed drive behavior: LED control during reads and policy persistence after a full power cycle. See the [retest record](../docs/UB4_MONITOR_08132_LED_RETEST.md).

## Build and wire

From the repository root in WSL/Linux, build a fresh candidate with the
[Monitor build wrapper](Build-Monitor-Fire24G-V0.8.1.sh). It applies the
passive core overlay and composes both plugins with the checked-in Monitor
inputs. A new build is a candidate until flashed and checked on hardware.

Use the [UB4 wiring table](../docs/FIRE24G-VERIFIED-WIRING.md#ub4-monitor-connections)
and [build/install guide](../docs/FIRE24G_V81_BUILD_AND_INSTALL.md). UB4 needs
all four passive capture wires. It has no write-protect wiring or authority.
