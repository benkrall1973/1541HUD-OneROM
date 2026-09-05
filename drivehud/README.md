# DriveHUD V0.0.30

## Hardware-Proven Passive 1541 Monitor

DriveHUD V0.0.30 is the hardware-proven passive UB4 monitor baseline for OneROM Fire-24-E and the Commodore 1541.

DriveHUD observes 1541 mechanism and DOS activity using RP2350 PIO/DMA capture and reports state to a Python GUI over USB CDC. The monitor is passive and never drives the 1541 bus.

## Proven Behavior

Hardware testing confirmed:

- Normal cycle-program track tracking.
- 1541 Diagnostic Cartridge manual half-steps.
- Exact `.0` / `.5` half-track display.
- Write-protect state.
- Density D0-D3 from VIA2 PB5/PB6.
- Motor and head-state display.
- Late GUI connection after drive boot.
- GUI reconnect while the drive remains powered.
- Immediate reconstruction of current track and write-protect state.

A power-cycle test with the head left at Track 25 confirmed that the GUI could be started after drive boot and immediately recover Track 25.

## Architecture

`1541 write cycles -> PIO0 SM3 -> DMA11 circular ring -> RP2350 decoder/state cache -> mailbox -> USB CDC -> Python GUI`

UB3 remains the normal ROM-serving OneROM. UB4 runs DriveHUD as a passive monitor.

## Important Invariants

- Do not casually retune the hardware-proven PIO/DMA acquisition.
- `$0022=1` is the strong HOME anchor.
- `$0022=0` is not physical Track 0.
- Non-home `$0022` values do not overwrite established phase tracking.
- No fixed bump distance.
- No Track-41 ceiling.
- Phase tracking does not freeze after HOME.
- Write protect comes from DOS LWPT `$001E` bit 4.
- Density comes from actual VIA2 ORB PB5/PB6 writes.
- Track and write-protect stabilization are GUI-only.
- Mailbox size remains 512 bytes.
- DMA ring remains at mailbox offset `+0x100`.

## Source Layout

- `drivehud/Build-DriveHUD-V030.ps1`
- `drivehud/gui/DriveHUD_V0.0.30_PIO_DMA.py`
- `plugins/user/drivehud-probe/`
- `plugins/system/usb/`

The USER and SYSTEM `drivehud_mailbox.h` files define the shared mailbox protocol and must remain synchronized.

## Canonical Source Build

From Windows PowerShell:

```powershell
.\drivehud\Build-DriveHUD-V030.ps1
```

The script:
1. Generates the deterministic 8192-byte `$FF` passive companion ROM.
2. Builds OneROM with `DRIVEHUD_PASSIVE_UB4`.
3. Builds the DriveHUD USER plugin.
4. Builds the USB SYSTEM plugin.
5. Composes Fire-24-E firmware.
6. Converts BIN to UF2.
7. Reports SHA-256 hashes.

Outputs are written to `build-drivehud/`.

## Proven Build Environment

- OneROM: v0.7.1
- Board: Fire-24-E
- ARM toolchain: `/usr/bin`
- picotool: `/opt/picotool/build/picotool`
- OneROM CLI: v0.3.0
- Host: Windows PowerShell + WSL

## Reproducibility

Two consecutive complete canonical builds in the proven environment produced identical artifacts:

### DriveHUD_OneROM_V0.0.30.bin
- Size: 204800 bytes
- SHA256: `42ff4f87191e36775d04862042da002d6b5e84bafc7eacf17710ed5d13bbaa8c`

### DriveHUD_OneROM_V0.0.30.uf2
- Size: 409600 bytes
- SHA256: `42e56bb6196ac75b491d1cd04fcc7a33862cb162d35c08762d6e40855f6b7bd0`

The OneROM composer embeds source paths in firmware metadata. Builds from different filesystem paths may therefore have different whole-file hashes even when the executable components are otherwise identical.

## External Dependencies

`picobootx` is pinned to `picobootx-rp2350-v0.1.0`.

TinyUSB and pico-sdkless are currently obtained without explicit commit pinning. The hashes above therefore demonstrate repeatability in the proven environment, not guaranteed bit-for-bit reproduction across future dependency revisions or arbitrary machines.

## GUI

The hardware-proven GUI is `drivehud/gui/DriveHUD_V0.0.30_PIO_DMA.py`.

It contains the V0.0.30 late-connect/DTR state-recovery behavior.

## Attribution

Designed and developed by Benjamin Krall with OpenAI ChatGPT assistance.

Copyright (c) 2026 Benjamin Krall
