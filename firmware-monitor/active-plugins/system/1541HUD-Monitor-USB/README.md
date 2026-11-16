# 1541HUD Monitor USB plugin 0.8.1.5

This is the UB4 Monitor system plugin. It exports the passive Monitor mailbox
over exclusive CDC for 1541HUD. It does not serve drive ROMs and must be paired
with Monitor user plugin 0.8.1.32 and a core built with
`HUD1541_PASSIVE_UB4`.

Build the complete Monitor image from the repository root in WSL/Linux:

```sh
export TOOLCHAIN=/path/to/arm-gnu-toolchain/bin
export ONEROM_CLI=/path/to/linux/onerom
bash firmware-monitor/Build-Monitor-Fire24G-V0.8.1.sh
```

The wrapper exports the pinned OneROM source, applies the passive UB4 overlay,
builds both Monitor plugins, composes the image, and inspects the result. The
output is a **candidate** at
`firmware-monitor/releases/v8.1/candidates/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.32.bin`.
The standalone plugin binary is not a complete image to flash. Do not build or
flash this plugin against the normal ROM-serving core.

See the [Monitor build and install record](../../../../docs/FIRE24G_V81_BUILD_AND_INSTALL.md),
[passive wiring guide](../../../../docs/FIRE24G-VERIFIED-WIRING.md), and
[.32 LED persistence record](../../../../docs/UB4_MONITOR_08132_LED_RETEST.md).
