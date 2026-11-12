# Fire-24G release checklist

Use this checklist before publishing a new firmware image, 1541HUD revision,
or GitHub Release. Record the source, artifacts and acceptance scope together
so later maintainers can identify the exact published configuration.

## Prepare

- [ ] Start from a clean checkout of the intended commit.
- [ ] Record the pinned upstream OneROM revision and every project overlay
  included in the build.
- [ ] Confirm that no optional DOS ROM collection, private manifest, serial
  number, local path, or unrelated binary is staged. Only the explicitly
  named stock baseline inputs/images are permitted; retain their notices.
- [ ] Run `./tools/maintenance/Test-Public-Package.ps1 -PackageRoot .`.
- [ ] Run `python tools/maintenance/Check-Documentation.py`.
- [ ] Confirm the README prerequisites and launch instructions work from a
  fresh Python installation, including the documented pyserial dependency.

## Build and inspect

- [ ] Build only with the documented WSL2/Linux source-build route.
- [ ] Record the exact build command, toolchain version, Rust/Cargo version,
  and OneROM CLI version.
- [ ] Inspect the produced image with the OneROM CLI.
- [ ] Calculate and record the SHA-256 for each distributed binary.
- [ ] Compare the image role, board target, plugins, and version string with
  the intended release notes.

## Hardware acceptance

### UB3 Control

- [ ] Flash one identified Fire-24G board using `--verify`.
- [ ] Confirm USB enumeration and the board serial.
- [ ] Verify ROM save/readback and IEC 8/9 save/readback.
- [ ] Verify write-protect OFF/ON/OFF if the optional circuit is fitted.
- [ ] Verify `ROMRESET`, wait for release, then perform an immediate directory
  read without an IEC stall.
- [ ] Test Apply+Save with UB3 USB-powered, then confirm controlled OneROM reboot
  activates the saved NV2 address without removing USB power.
- [ ] Test Apply Only at a different address, then power-cycle the 1541 while
  USB data stays connected through a cable with host VBUS +5 V blocked. Confirm
  UB3 also restarts and loads the saved NV2 address. Alternatively power-cycle
  UB3 explicitly. A standard powered USB cable may keep UB3 alive and retain
  the temporary SEL_A address.

### UB4 Monitor

- [ ] Flash one identified Fire-24G board using `--verify`.
- [ ] Confirm green idle LED, density colours while reading, header/SYNC/RPM
  telemetry, and no effect on drive operation.
- [ ] Confirm LED policy save/readback if that feature changed.

### 1541HUD

- [ ] Confirm board-role assignment by serial, not COM port number.
- [ ] Confirm Control cards, Monitor cards, scrolling/navigation, and LED
  controls behave correctly after reconnect and refresh.

## Publish

- [ ] Create an annotated Git tag such as `v8.1-fire24g-baseline`.
- [ ] Create a GitHub Release from that tag; attach the reviewed binaries.
- [ ] Include board target, role, SHA-256, pinned upstream status, acceptance
  results, known limitations, and upgrade notes.
- [ ] State plainly if the release was not built from a clean independent host.
- [ ] Do not call an upstream development snapshot an official upstream release.

