# Public v8.1 maintenance review

This records the preceding documentation-maintenance release. The subsequent
[supported-source security cleanup](SOURCE_SECURITY_CLEANUP_2026-11-10.md)
changes the active host-source packaging and supersedes the preserved-source
alert boundary described below. Firmware images remain unchanged.

This maintenance pass updates documentation and public packaging around the
existing tested baseline. It does not change the firmware, plugins, vendored
upstream source, Windows CLI executable or HUD application code.

## Changes

- Document Python, Tcl/Tk and pyserial installation before launching HUD.
  Runtime requirements pin pyserial 3.5, matching the maintainer's environment.
- Distinguish the Python HUD application from the bundled OneROM CLI EXE.
- Correct stock ROM baseline notices and the public package exceptions;
  optional third-party DOS collections remain excluded.
- Update the active Monitor plugin README to v0.8.1.32 and the supported
  complete-role build wrapper; repair relocated Host-Control reference links.
- State consistently that automatic power sensing was dropped from planned
  work, because saved settings reload through the USB-powered reboot path.
- Document the specified KSC2383YTA E-C-B pin numbering and physical WP status
  masking while override is active.
- Add a project Markdown file-target check to CI. Pinned upstream documents
  remain unchanged.

## Evidence and retained boundaries

All 51 HUD regression tests passed locally. Release firmware hashes and the
Windows CLI hash match the preceding baseline. Public CI still runs full ARM
builds for both role cores and all four custom plugins, host CLI regressions,
Windows CLI builds, native firmware regressions and package validation.

The exact public stock Control image has 47 recorded loose-board checks;
actual drive operation and complete USB power-cycle qualification remain
pending at the time of that maintenance pass. The subsequent stock two-ROM
drive test and its limits are recorded in
STOCK_CONTROL_DRIVE_TEST_2026-11-10.md. The cosmetic invalid-IEC response still
advertises MAX=11 while accepting only 8/9.

Monitor .32 retains its recorded drive acceptance and existing test scope.
The C64 configurator remains deferred. Prototype interface PCB work is backed
up separately in the private repository; it is not public qualified hardware.

See [the current runbook](V81_BASELINE_RUNBOOK.md),
[qualification matrix](VALIDATION_MATRIX.md),
[stock Control bench results](STOCK_CONTROL_LOOSE_TEST_2026-11-10.md),
and [current release](https://github.com/benkrall1973/1541HUD-OneROM/releases/latest).
