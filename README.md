# 1541HUD / OneROM Fire-24G

1541HUD adds USB control and optional drive monitoring to a OneROM Fire-24G
installation in a Commodore 1541. The **UB3 Control** board serves drive ROMs,
sets the IEC address, resets the drive, manages optional write-protect control,
and sets LED behavior. The optional **UB4 Monitor** board passively reports
motor and track activity, RPM, disk density, SYNC, and validated disk-header
activity; it does not control the drive.

1541HUD is the Windows dashboard for those boards. It combines live drive
telemetry with direct Control-board actions, so it can show what the drive is
doing while giving you a place to change its supported settings. The Monitor
is optional: ROM serving and Control functions run on UB3 without UB4.

## What 1541HUD does

### Monitor the drive

With a correctly wired **UB4 Monitor**, 1541HUD can display and log activity
captured from the drive while it runs:

- **Motor and rotation:** motor state and measured RPM, with RPM derived from
  physical SYNC edges over complete revolutions.
- **Head position:** stepper phase and half-track movement, with the DOS track
  state used as the home reference.
- **Disk format activity:** the drive's selected density zone (D0–D3), plus
  checksum-validated disk-header activity when it is observed.
- **Drive state:** SYNC activity and the write-protect state reported through
  DOS RAM telemetry.
- **Activity log:** review the events received by the HUD. Log filters control
  which events are shown; they do not disable Monitor capture.

The Monitor is **passive telemetry**. UB4 does not serve drive ROMs, reset the
drive, drive the write-protect circuit, or issue commands on the 1541 bus. Its
telemetry requires the four UB4 capture connections documented in the
[wiring guide](docs/FIRE24G-VERIFIED-WIRING.md#ub4-monitor-connections). It is
not a general-purpose disk analyzer and should not be read as a guarantee that
every disk operation or header was captured.

### Control the drive

The **UB3 Control** board provides the drive-facing functions in 1541HUD:

- **Select a drive ROM profile.** The HUD presents the profile names defined
  by the installed ROM image. The image determines their order; hidden profile
  labels stay hidden without changing the underlying slot selection.
- **Set the IEC device address.** On the documented Fire-24G wiring, the
  supported choices are device 8 and 9. **Apply Only** changes the live
  address without saving it; **Apply and Save** stores the setting for later
  startup. Apply Only requires Control USB plugin 0.8.1.17 or newer.
- **Reset the drive.** HUD confirms the action before pulsing the wired active-
  low reset line. Reset does not change the ROM, IEC address, or write-protect
  settings.
- **Manage optional write protection.** If the documented transistor and
  sense circuit is installed, HUD can show write-protect status and request a
  writable override or restore normal protection. HUD verifies the requested
  state before updating the card.
- **Configure board LEDs.** Control and Monitor LED policies are managed
  separately; supported boards can use automatic behavior or turn their
  indicator lights off. Policies are saved on the corresponding OneROM board.
- **Assign board roles.** HUD associates Control and optional Monitor roles
  with USB serial identities, rather than relying on COM port numbers that can
  change after reconnecting.

The HUD also provides connection/status feedback for the selected boards and
surfaces unavailable or unsupported controls when the connected firmware does
not provide them. The exact actions shown depend on the board, installed
firmware, and optional wiring. See [1541HUD setup and compatibility](1541hud/README.md)
for version requirements and connection details.

### How the parts fit together

| Part | Role | What it does |
| --- | --- | --- |
| **UB3 Control** | Drive control and ROM serving | Serves the selected drive ROM, handles configured IEC address and reset functions, and supports optional write-protect control. |
| **UB4 Monitor** | Passive capture | Samples documented drive signals and reports telemetry to the HUD; it does not control the drive. |
| **1541HUD for Windows** | Dashboard | Displays Monitor data and sends supported settings/actions to the assigned Control or Monitor board over USB. |
| **OneROM CLI** | Setup and maintenance | Scans for boards, inspects and programs firmware, and verifies flashed images. It is separate from the HUD application. |

Control and Monitor firmware images are different board roles and are not
interchangeable. See the [validation matrix](docs/VALIDATION_MATRIX.md) for
what has been exercised on hardware and the boundaries of that evidence.

### C64 OneROM configurator

The repository also includes the **1541 OneROM Selector R20 V1.4.31** for
selecting a catalogued drive-ROM profile and IEC address from a Commodore 64.
Its BASIC source, tokenizer, reproducible build wrapper, PRG, checksums, and
technical guide are in [the configurator package](<C64 Configurator/README.md>).
The V1.4.31 PRG rebuilds byte-for-byte from the tracked source; its final
screen interaction has not yet been separately confirmed on hardware.

## Drive ROMs you can use

The public [r21 Basic image](https://github.com/benkrall1973/1541HUD-OneROM/releases/tag/v8.1-fire24g-ub3-images-2026-11-16)
contains four selectable drive ROM profiles. Choose the matching C64 KERNAL
for fast DOS features; a drive ROM and a C64 KERNAL are separate things.

| Drive profile | Matching C64 KERNAL | Extra hardware / notes |
| --- | --- | --- |
| **DOS 2.6 Original** | Stock Commodore KERNAL | Standard IEC. Default startup profile. |
| **DOS 2.6 1541-II** | Stock Commodore KERNAL | Standard IEC; this profile was tested in a longboard 1541. |
| **SpeedDOS** | English SpeedDOS+ KERNAL | Parallel cable for fast transfer. IEC works with the parallel fast path disabled. |
| **DolphinDOS 2.0** | DolphinDOS 2.0 KERNAL | Parallel cable and compatible drive-side RAM for its fast path. |

The r21 image starts with **DOS 2.6 Original** and IEC address **8**. It omits
JiffyDOS and S-JiffyDOS. It has been flashed and tested in a drive: the owner
confirmed Commodore DOS, SpeedDOS, DolphinDOS 2.0, parallel cable operation,
IEC address control, and write-protect status and override. See the
[r21 hardware test record](docs/R21_BASIC_HARDWARE_TEST_2026-11-16.md).
All four r21 drive profiles use the three-page **C/A/E layout** and require
the documented **74LS138 decoder wiring** (X1 to `/Y5`, pin 10; X2 to `/Y7`,
pin 7). The complete ROM composition is in
[`releases/r21-basic-no-jiffydos-compose.json`](releases/r21-basic-no-jiffydos-compose.json).
Use the stock two-ROM image for a direct X1 socket-wiring setup.

The separate [stock two-ROM image](https://github.com/benkrall1973/1541HUD-OneROM/releases/tag/v8.1-fire24g-ub3-images-2026-11-16)
is the hardware-tested starting point. It contains stock 1541 DOS 2.6 and the
OneROM bootloader.

### JiffyDOS and S-JiffyDOS

You can make private images from ROMs and tools you are licensed to use. The
public repository includes preparation notes and project-authored material,
but **does not include licensed JiffyDOS ROMs or a ready-made JiffyDOS image**.
JiffyDOS fast serial transfers need the matching licensed JiffyDOS C64 KERNAL.
S-JiffyDOS uses RAMBoard shadow RAM at `$8000–$9FFF`; software that writes to
that range while S-JiffyDOS is active can corrupt its running code. See the
[JiffyDOS](roms/jiffydos/README.md), [S-JiffyDOS](roms/s-jiffydos/README.md),
and [RAMBoard](roms/ramboard/README.md) notes.

Browse the [ROM and KERNAL library](roms/README.md) for included pages, source,
build notes, and checksums. Third-party ROM assets are not covered by this
project's MIT license; review the [asset provenance and rights notes](roms/ASSETS.md)
before reusing or redistributing them. C64 KERNALs are installed on the C64;
the UB3 images above contain drive-side ROM profiles, not a C64 KERNAL replacement.

## Get started

1. Read the [baseline runbook](docs/V81_BASELINE_RUNBOOK.md) and
   [verified wiring guide](docs/FIRE24G-VERIFIED-WIRING.md). These images are
   for Fire-24G boards; UB3 Control and UB4 Monitor images are not interchangeable.
   Match the r21 C/A/E decoder requirement above to your physical wiring before flashing.
2. Close 1541HUD, connect only the board you intend to program, enter BOOTSEL,
   and scan for its serial:

   ```powershell
   ./tools/onerom-v0.6.0-hud1.exe --unrecognised scan --board fire-24-g
   ```

3. Confirm the serial and choose the image for that board. Download UB3 Control
   images from the
   [release page](https://github.com/benkrall1973/1541HUD-OneROM/releases/tag/v8.1-fire24g-ub3-images-2026-11-16).
   The UB4 Monitor image is a
   [separate repository file](releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.32.bin),
   with its hash in the [release manifest](releases/v8.1-manifest.json). Then
   flash the selected image with `program --verify`:

   ```powershell
   ./tools/onerom-v0.6.0-hud1.exe --serial <SERIAL> program `
     --firmware <PATH-TO-IMAGE.bin> --verify
   ```

   Compare the image's SHA-256 with the hash listed for that exact file in the
   release page or manifest.
4. Install and run the Windows HUD by following
   [1541HUD setup](1541hud/README.md). V0.8.0.58 is the current public source
   candidate; V0.8.0.51 remains the last hardware-qualified HUD baseline. The
   public package includes Python source and launchers, not a standalone HUD
   executable. Apply Only needs UB3 Control USB 0.8.1.17 or newer (the current
   repository source is 0.8.1.18). For a USB-connected power cycle to reload
   the saved address, UB3 must lose power too: use a data cable that passes
   D+/D− and ground while blocking host +5 V, or power-cycle UB3 separately.

## Build or customize

- To build the Control firmware, follow [Build UB3 Control](docs/BUILD_UB3_CONTROL.md).
- To compose a custom drive image, start with the [ROM library](roms/README.md),
  [page preparation guide](docs/THIRD_PARTY_DOS_PAGE_PREPARATION.md), and
  [composition guide](docs/UB3_CONTROL_CLI_COMPOSITION_GUIDE.md). Examples
  are in `roms/configs/`.
- Keep private ROM inputs and custom images out of public Git. JiffyDOS is
  commercial software; use only ROMs and tools you are licensed to use.

## Repository map

| Folder | Contents |
| --- | --- |
| `releases/` | UB3 and UB4 firmware images and composition manifests |
| `1541hud/` | Windows HUD source and launch/build files |
| `firmware-control/` | UB3 Control firmware and plugins |
| `firmware-monitor/` | UB4 passive Monitor firmware and plugins |
| `roms/` | DOS ROM pages, KERNALs, references, and ROM-free config examples |
| `C64 Configurator/` | C64 selector source, build tools, release PRG, and checksums |
| `docs/` | Wiring, build, flashing, and verification guides |
| `source/` | Supported OneROM source snapshot and provenance |

The supported base is the official OneROM v0.8.1 release. See the
[source provenance](source/one-rom-v0.8.1/1541HUD-SOURCE.json),
[contribution guide](CONTRIBUTING.md), and [safety notes](SECURITY.md).

Disconnect power before changing wiring. Do not connect 5 V drive signals
directly to 3.3 V OneROM GPIO. Verify the board role and serial before flashing.
