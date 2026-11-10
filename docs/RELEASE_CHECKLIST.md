# Fire-24G release checklist

Use this checklist before publishing a new firmware image, 1541HUD revision,
or GitHub Release. A release is a promise to other people; do not turn it into
an archaeological dig later.

## Prepare

- [ ] Start from a clean checkout of the intended commit.
- [ ] Record the pinned upstream OneROM revision and every project overlay
  included in the build.
- [ ] Confirm that no third-party DOS ROM, private manifest, serial number,
  local path, or unrelated binary is staged.
- [ ] Run `./tools/maintenance/Test-Public-Package.ps1 -PackageRoot .`.

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
- [ ] Test Apply+Save while OneROM remains powered by normal USB, then confirm saved NV
  configuration reloads.

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

- [ ] Create an annotated Git tag such as `v8.1.0-fire24g-r1`.
- [ ] Create a GitHub Release from that tag; attach the reviewed binaries.
- [ ] Include board target, role, SHA-256, pinned upstream status, acceptance
  results, known limitations, and upgrade notes.
- [ ] State plainly if the release was not built from a clean independent host.
- [ ] Do not call an upstream development snapshot an official upstream release.

