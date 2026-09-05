# DriveHUD V0.0.30

## Hardware-Proven Passive 1541 Monitor

DriveHUD V0.0.30 is the hardware-proven passive UB4 monitor baseline for OneROM Fire-24-E and the Commodore 1541.

DriveHUD observes 1541 mechanism and DOS activity using RP2350 PIO/DMA capture and reports state to a Python GUI over USB CDC. The monitor is passive and never drives the 1541 bus.

The hardware-proven runtime is preserved by tag `v0.0.30` at commit `73769e1`. Later commits on the DriveHUD source branch contain release, CI, documentation, build, and WSL portability hardening without redefining the runtime as V0.0.31.

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

The build script supports both normal Windows-mounted WSL paths such as `/mnt/c/...` and WSL UNC paths such as `\\wsl$\Ubuntu2204\...`, including PowerShell provider-prefixed paths returned by `Resolve-Path`.

## Proven Build Environment

- OneROM: v0.7.1
- Board: Fire-24-E
- ARM toolchain: `/usr/bin`
- picotool: `/opt/picotool/build/picotool`
- OneROM CLI: v0.3.0
- Host: Windows PowerShell + WSL

## Reproducibility

Two consecutive complete canonical builds in the original proven Desktop environment produced identical artifacts:

### DriveHUD_OneROM_V0.0.30.bin
- Size: 204800 bytes
- SHA256: `42ff4f87191e36775d04862042da002d6b5e84bafc7eacf17710ed5d13bbaa8c`

### DriveHUD_OneROM_V0.0.30.uf2
- Size: 409600 bytes
- SHA256: `42e56bb6196ac75b491d1cd04fcc7a33862cb162d35c08762d6e40855f6b7bd0`

A completely fresh GitHub clone was also built twice from `/tmp/DriveHUD-OneROM-fresh`. Both builds completed successfully, produced identical same-path outputs, and left the tracked working tree clean.

Fresh-clone hashes were:

- BIN SHA256: `6d9e59e3436f85899b68e42a3b2c1623a23a0494519f90a6223a3968cab67a25`
- UF2 SHA256: `d50b21e8bdad7acd654bd9e70e41d029a4ff7bf23e8fb084e8dd79dcbe710c35`

The OneROM composer embeds source paths in firmware metadata. Builds from different filesystem paths may therefore have different whole-file hashes even when built from the same source. Same-path deterministic rebuilding is proven; cross-path whole-image hash equality is not expected.

## External Dependencies

`picobootx` is pinned to `picobootx-rp2350-v0.1.0`.

TinyUSB and pico-sdkless are currently obtained without explicit commit pinning. Their exact hardware-proven revisions are not known, so they are intentionally not pinned to guessed commits.

The verified state therefore demonstrates functional source-build reproducibility and same-path deterministic rebuilding in the tested environment, not guaranteed bit-for-bit reproduction across arbitrary future dependency revisions, machines, or filesystem paths.

## GUI

The hardware-proven GUI is `drivehud/gui/DriveHUD_V0.0.30_PIO_DMA.py`.

It contains the V0.0.30 late-connect/DTR state-recovery behavior.

## Attribution

Designed and developed by Benjamin Krall with OpenAI ChatGPT assistance.

Copyright (c) 2026 Benjamin Krall
