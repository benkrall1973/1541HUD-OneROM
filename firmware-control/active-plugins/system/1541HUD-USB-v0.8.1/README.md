# 1541HUD Control USB Plugin (OneROM V8.1)

This is the 1541HUD Control system plugin for OneROM **V8.1**. It is installed
in fixed plugin slot 0 and provides the normal OneROM USB runtime, picobootx
service, CDC logging/input, ROM-list discovery, and selector transport.

## Build

From this directory, run:

```bash
make TOOLCHAIN=/usr/bin
```

Set `ONEROM_ROOT` to the Control work tree created by the supported Control
wrapper:
`firmware-control/.work/control-core-v0.8.1/one-rom-v0.8.1`.
The wrapper copies the root-level official v0.8.1 source into that work tree.
The output is:

```text
build/1541-OneROM-1541HUD-USB-v0.8.1.10.bin
```

Do not pair it with another core line. The V8 Control image also requires the
documented idle-yield core integration for persistent NV writes.

## V8.1 HUD transport

Build `v0.8.1.10` includes the V8.1 USB-DPRAM reservation required by the
firmware before it can place its DPRAM log and allocator pool. It also enables
the exclusive-serial property so the Control
protocol owns the CDC stream; core logs are retrieved through picoboot/DPRAM
instead of interleaving with HUD replies. Selector replies are queued and
drained across USB task passes, since a TinyUSB flush begins a transfer rather
than immediately freeing the 64-byte FIFO. This is required for reliable
`ROMGET`, `ROMSET`, and `ROMIEC` feedback in 1541HUD V0.8.0.22.

## Persisted front-panel LED policy

`ROMLED=OFF` saves the HUD front-panel policy in NV byte 1 and immediately
turns **both** the status channel and RGB channel off. `ROMLED=AUTO` saves the
automatic policy, restores the normal status channel, and restores the
per-ROM RGB colour. `ROMLED?` reports the saved policy. The policy is applied
during plugin initialization, so it survives a cold reboot. While OFF is
saved, ROM-selection and IEC-save feedback animations are suppressed.

This paired policy is deliberate for Fire-24G: its status and RGB logical
channels share the physical LED GPIO, so controlling just one channel can
create false blended colours. A firmware-level fault indication remains under
core control and is not claimed by this plugin.

## Eight-profile selector limit

The current UB3/74LS138 collection has eight selectable ROM profiles plus the
bootloader. The selector therefore accepts `ROMSET=1` through `ROMSET=8` and
rejects any other saved-slot value. NV0 selection saves are expected to work
while the C64 and 1541 are running; the active ROM is RAM-backed. A C64 cursor
lock during a completed `ROMSET` write is a regression, not expected behavior.

## OneROM contribution identity

The Studio/OneROM contribution name is `1541-1541hud-usb`. Its metadata is in
`plugin-meta.json`; the binary header, rather than the metadata, is the
authority for type, version, and minimum firmware compatibility.

## UB3 / 74LS138 three-page ROM sets

The USB ROM-list path supports both older two-page groups and the long-board
three-page groups used by the current Control configuration. For a three-page
set the image order is fixed: UB3 native `/CS` = `$C000-$DFFF`, X1 =
`$A000-$BFFF`, and X2 = `$E000-$FFFF`. The USB slot display searches every
member for the configured human label rather than assuming the final member is
the right one.

## Write-protect override

For the UB3/74LS138 configuration, `SEL_C` is reserved for the transient
write-protect override. Startup resolves `SEL_C` from metadata (GPIO25 on
Fire-24G) and drives it LOW so the
external override is physically OFF after every power cycle; `ROMWP=ON` drives
it high and `ROMWP=OFF` drives it low. The override circuit is SEL_C through a
5.1 kOhm resistor to the KSC2383YTA NPN base, emitter to 1541 ground, and
collector to 1541 P6 pin 12.

`SEL_D` (GPIO24 on Fire-24G) is reserved as an optional **input-only** physical write-protect
sensor. Wire P6 pin 12 through a 22 kOhm upper resistor to the SEL_D node, and
fit a 22 kOhm lower resistor from that node to ground. GPIO27 is 3.3 V-only;
never connect P6 pin 12 directly. `ROMWP?` reports `SENSE_AVAILABLE` and
`PHYSICAL=PROTECTED` or `PHYSICAL=WRITABLE` when the override is off. It reports
`PHYSICAL=MASKED` while the override is on because SEL_C deliberately forces
the same P6 node low. X1 and X2 must never be used for this function: they
select the DD2 A-ROM and E-ROM pages.

The P6-pin-12 node is intentionally shared: the disk sensor feeds it, the
SEL_C NPN collector can pull it low to force writable, and the SEL_D divider
observes it. **Do not use UC2 pin 12** as the sense source; it is 6522 PB2
(motor control). UC2 pin 14/PB4 is the downstream write-protect input, but
P6 pin 12 is the preferred source and avoids another wire to the main PCB.
SEL_D sensing works with Control OneROM alone; a Monitor OneROM is optional
and contributes unrelated passive drive telemetry.

## Fire-24G C46 reset wiring

On Fire-24G, `SEL_B` is GPIO27. Connect it through a **1 kOhm** series
resistor to C46 positive (or CR7 anode) on the 1541 long board. This was
hardware-verified: the older 2.2 kOhm value only pulled C46 to about 1.2 V;
1 kOhm pulls it to about 0.8 V and reliably resets the drive. The plugin holds
SEL_B low for 350 ms for `ROMRESET`, then releases it to input. During a
OneROM boot/reboot, the core plus plugin hold it for 350 ms so the 1541 cannot
execute while its ROM server is restarting. Never drive SEL_B high.
