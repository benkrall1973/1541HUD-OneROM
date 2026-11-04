# 1541HUD Monitor USB plugin (OneROM V8.0)

This is the dedicated UB4 system USB plugin. It exports the passive Monitor
mailbox over CDC for 1541HUD; it is not the ROM-serving Control USB plugin.

Build from this directory:

```sh
make TOOLCHAIN=/usr/bin -j2
```

The result is `build/1541-OneROM-1541HUD-Monitor-USB-v0.8.0.7.bin`. Pair it
only with `1541-1541HUD-Monitor` and a core built with
`HUD1541_PASSIVE_UB4`. The final composition and diagnostic record contract
are documented in `firmware-monitor/docs/TECHNICAL_REFERENCE.md`.
