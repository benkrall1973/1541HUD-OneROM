# 1541HUD passive Monitor plugin (OneROM V8.0)

This is the UB4 user plugin. It captures 1541 activity through PIO0 state
machine 3 and DMA channel 11, then writes decoded events to the Monitor
mailbox. It is safe only with the V8 core built using `HUD1541_PASSIVE_UB4`.

Build from this directory:

```sh
make TOOLCHAIN=/usr/bin -j2
```

The result is `build/1541-OneROM-1541HUD-Monitor-v0.8.0.7.bin`. Pair it only
with the Monitor USB system plugin and the passive Monitor base documented in
`firmware-monitor/docs/TECHNICAL_REFERENCE.md`.
