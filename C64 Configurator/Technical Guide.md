# C64 OneROM Configurator — Technical Guide

## Release and validation status

The tracked public baseline is **1541 OneROM Selector R20 V1.4.31**. The
source and PRG are recorded in
[`manifest.json`](manifest.json),
and the repository build wrapper reproduces the PRG byte-for-byte.

The owner previously reported that the V1.4.24 service scan displayed all six
programmed ROM sets. V1.4.31 retains that scan and service-start sequence.
The final V1.4.31 menu redraw changes have not been separately confirmed on a
C64 in the project record. Treat build reproducibility and hardware
qualification as separate evidence; record a new hardware result before
claiming the exact V1.4.31 interface is fully qualified.

## What the program does

The program runs on a Commodore 64 and communicates over IEC with a 1541
OneROM service. It uploads a small driver/helper to the drive, starts and
queries the OneROM service, reads the configured user-ROM catalog, and offers
menus for selecting the active/boot ROM profile and IEC address.

It is a **selector and service client**, not a ROM composer or flasher. It
does not create profiles, write drive-ROM pages, flash OneROM firmware, or
install a C64 KERNAL. The catalog and supported socket layout come from the
OneROM firmware image and its wiring.

## Compatibility and limits

- The program identifies itself as **R20 V1.4.31**, **OneROM FW V0.8.1**, and
  **RBCP 0.1.2+**. Use a OneROM build that provides the expected service
  protocol and catalog records.
- The startup path probes the OneROM service in the upper socket first, then
  retries the lower socket if the upper probe does not find the service.
- The catalog UI supports up to **eight visible user ROM sets**. It omits
  catalog names beginning with `@HIDDEN` and retains the remaining profile
  mapping when presenting them.
- A catalog profile may represent a one-page, two-page C/E, or three-page
  C/A/E ROM arrangement when that profile is supported by the installed
  firmware and matching hardware wiring. The selector displays the service's
  profile names; it does not validate the ROM pages or decoder wiring.
- IEC device addresses **8–11** are selectable in the address menu. Whether
  10 and 11 are available depends on the actual wiring. The normal polling
  screen finds IEC devices 8–11 and allows manual selection before continuing.
- No third-party ROM data is included. The root MIT license does not grant
  rights to Commodore, JiffyDOS, or other ROM contents.

For supported Fire-24G wiring and board roles, see the
[verified wiring guide](../docs/FIRE24G-VERIFIED-WIRING.md) and
[validation matrix](../docs/VALIDATION_MATRIX.md). For firmware image composition,
see the [ROM-free composition guide](../roms/configs/README.md).

## Startup and discovery sequence

1. The C64 polls IEC addresses 8–11. The polling screen shows a ten-second
   automatic selection countdown; Up/Down changes the highlighted device,
   Return continues, and Q exits.
2. The program opens the selected drive command channel and uploads its helper
   routines using the drive's memory-write command.
3. It probes for OneROM in the upper socket first. If that probe does not find
   OneROM, it reconfigures the helper for the lower socket and probes again.
4. Once the service starts, it queries catalog metadata and the user ROM-set
   names. An empty catalog or service/query error stops before the selector is
   shown.
5. The menu reports the number of visible sets and marks the active profile
   in the status line. The menu shows profile labels without slot numbers; the
   firmware's catalog mapping is retained internally.

The upper/lower probe describes where the OneROM service is found in the
drive's ROM sockets. It is independent of whether a selectable profile uses
one, two, or three ROM pages.

## ROM selection screen

| Key | Action |
| --- | --- |
| Up / Down | Move the selection through the visible catalog; selection wraps at either end. |
| S | Save the selected profile as the boot profile and reboot the drive. |
| I | Open the IEC address screen. |
| Q | Quit. |

The `ACTIVE ROM = ...` label reports the profile currently active in the drive.
The highlight shows the profile currently selected in the menu. Saving writes
the selected catalog slot as the boot choice; merely moving the highlight does
not save it.

## IEC address screen

The screen separately shows the active IEC address and saved boot address. The
available choices are rendered as `[08]`, `[09]`, `[10]`, and `[11]` where
supported by wiring.

| Key | Action |
| --- | --- |
| Up / Down | Select an address from 8 through 11. |
| 8 / 9 | Select address 8 or 9 directly. |
| 1 then 0 / 1 | Select address 10 or 11. |
| S | Save the selected boot address to NV2, read it back, and verify it. |
| C | Apply the selected address temporarily after a Yes/No confirmation. |
| R | Return to ROM selection. |
| Q | Quit. |

Temporary Apply changes the active address through the OneROM service without
changing the saved boot address. Save is the persistent boot choice. The menu
displays “Availability depends on wiring” because address decoding is a
hardware property; the selector cannot make an unwired address respond.

## Build and artifact verification

From the repository root, run:

```powershell
& "./C64 Configurator/Build-C64-OneROM-Configurator.ps1"
```

The wrapper uses `py -3` when the Windows Python launcher is installed and
falls back to `python`. The tokenizer is checked in at
[`source/scripts/tokenize_basic_v2.py`](source/scripts/tokenize_basic_v2.py)
and has no third-party Python dependencies. Build output goes to the ignored
package-local `build/` folder. The wrapper verifies source, tokenizer, and PRG
hashes against the tracked manifest and fails if a rebuilt PRG differs from
the published artifact.

The public package includes:

- [ASCII BASIC source](source/C64-OneROM-Configurator-r20-V1.4.31.bas)
- [Tokenizer](source/scripts/tokenize_basic_v2.py)
- [Build wrapper](Build-C64-OneROM-Configurator.ps1)
- [V1.4.31 PRG](releases/C64-OneROM-Configurator-r20-V1.4.31.PRG)
- [SHA-256 and size manifest](manifest.json)

## Troubleshooting

### No IEC devices appear

Confirm the drive is powered, the IEC cable is connected, and the drive is
using an address between 8 and 11. Use Up/Down to select a responding device
and Return to continue. Address 10/11 support depends on the wiring.

### The program reports OneROM not found or stalls before the menu

Confirm the selected drive is the OneROM-equipped 1541, the service firmware
matches the R20 selector protocol, and the OneROM socket wiring is correct.
The program probes upper then lower socket. A service startup/catalog error
means the selector did not receive a usable service response; it is not proof
that the ROM catalog is empty. Power-cycle the drive/OneROM and retry after
confirming the IEC address.

### A configured ROM set is missing from the list

The selector can show at most eight visible user profiles. Names prefixed
`@HIDDEN` are intentionally omitted. Verify the profile appears in the
firmware's catalog and is not marked hidden; the C64 utility does not create
or repair catalog entries.

### Address 10 or 11 does not respond

The menu can display the address, but physical availability depends on the
decoder and IEC wiring. Use a wiring-supported address; software selection
cannot substitute for the required hardware signals.
