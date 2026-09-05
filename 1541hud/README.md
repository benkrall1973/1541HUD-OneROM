# 1541HUD

## Passive Commodore 1541 Monitor for OneROM

1541HUD is the renamed continuation of the project originally developed as **DriveHUD**. It is a passive Commodore 1541 monitor for OneROM Fire-24-E.

The hardware-proven runtime remains **DriveHUD V0.0.30**, preserved by tag `v0.0.30` at commit `73769e1`. That tag is historical and immutable. The current rename work is a **1541HUD V0.0.31 candidate** until the renamed build passes CI and is verified on real hardware.

1541HUD observes 1541 mechanism and DOS activity using RP2350 PIO/DMA capture and reports state to a Python GUI over USB CDC. The monitor is passive and never drives the 1541 bus.

## Proven V0.0.30 Behavior

Hardware testing confirmed:

- Normal cycle-program track tracking
- 1541 Diagnostic Cartridge manual half-steps
- Exact `.0` / `.5` half-track display
- Write-protect state
- Density D0-D3 from VIA2 PB5/PB6
- Motor and head-state display
- Late GUI connection after drive boot
- GUI reconnect while the drive remains powered
- Immediate reconstruction of current track and write-protect state

## Architecture

`1541 write cycles -> PIO0 SM3 -> DMA11 circular ring -> RP2350 decoder/state cache -> mailbox -> USB CDC -> Python GUI`

UB3 remains the normal ROM-serving OneROM. UB4 runs 1541HUD as a passive monitor.

## Important Invariants

The rename does not authorize functional redesign. In particular:

- Do not casually retune the hardware-proven PIO/DMA acquisition.
- `$0022=1` is the strong HOME anchor.
- `$0022=0` is not physical Track 0.
- Non-home `$0022` values do not overwrite established phase tracking.
- No fixed bump distance.
- No Track-41 ceiling.
- Phase tracking does not freeze after HOME.
- Write protect comes from DOS LWPT `$001E` bit 4.
- Density comes from actual VIA2 ORB PB5/PB6 writes.
- Mailbox size remains 512 bytes.
- DMA ring remains at mailbox offset `+0x100`.

## Source Layout

- `1541hud/Build-1541HUD-V031.ps1`
- `1541hud/gui/1541HUD_V0.0.31_PIO_DMA.py`
- `plugins/user/1541hud-probe/`
- `plugins/system/usb/`

The USER and SYSTEM `1541hud_mailbox.h` files define the shared mailbox protocol and must remain byte-identical.

Thin `drivehud_mailbox.h` compatibility headers remain temporarily for the hardware-proven acquisition/USB implementation. They only alias legacy C identifiers to the renamed mailbox interface; they do not change the mailbox address, magic, size, ring offset, or event encoding.

## Canonical Candidate Build

From Windows PowerShell:

```powershell
.\1541hud\Build-1541HUD-V031.ps1
```

The script:

1. Generates the deterministic 8192-byte `$FF` passive companion ROM.
2. Builds OneROM with `HUD1541_PASSIVE_UB4`.
3. Builds the 1541HUD USER plugin.
4. Builds the USB SYSTEM plugin.
5. Composes Fire-24-E firmware.
6. Converts BIN to UF2.
7. Reports SHA-256 hashes.

Outputs are written to `build-1541hud/`.

## Proven Build Environment

- OneROM: v0.7.1
- Board: Fire-24-E
- ARM toolchain: `/usr/bin`
- picotool: `/opt/picotool/build/picotool`
- OneROM CLI: v0.3.0
- Host: Windows PowerShell + WSL

## Historical V0.0.30 Reproducibility

The following hashes belong to the hardware-proven DriveHUD V0.0.30 source/build environment and are retained for historical verification. They are **not** expected to match the renamed candidate because source paths and metadata names have changed.

Original Desktop build:

- BIN size: 204800 bytes
- BIN SHA256: `42ff4f87191e36775d04862042da002d6b5e84bafc7eacf17710ed5d13bbaa8c`
- UF2 size: 409600 bytes
- UF2 SHA256: `42e56bb6196ac75b491d1cd04fcc7a33862cb162d35c08762d6e40855f6b7bd0`

Fresh-clone V0.0.30 hashes:

- BIN SHA256: `6d9e59e3436f85899b68e42a3b2c1623a23a0494519f90a6223a3968cab67a25`
- UF2 SHA256: `d50b21e8bdad7acd654bd9e70e41d029a4ff7bf23e8fb084e8dd79dcbe710c35`

OneROM embeds source/path metadata, so cross-path whole-image hash equality is not expected.

## External Dependencies

`picobootx` is pinned to `picobootx-rp2350-v0.1.0`.

TinyUSB and pico-sdkless are not pinned to known historical commits. Do not invent pins for them merely to make a reproducibility claim look cleaner than reality.

## Attribution

1541HUD is built on OneROM v0.7.1 by Piers Finlayson. See the repository root README and `LICENSE.md` for upstream attribution and licensing details.

1541HUD project development: Benjamin Krall, with OpenAI ChatGPT assistance.
