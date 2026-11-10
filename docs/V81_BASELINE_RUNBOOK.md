# v8.1 consolidated baseline

This is the public default branch and the single active Fire-24G baseline.
It combines Control, Monitor and HUD V51.

| Component | Version |
| --- | --- |
| OneROM base | Official v0.8.1, commit 1b189260d85a5feced19e969399986f5f1d1cabe |
| Control USB / Host-Control | 0.8.1.16 / 0.8.1.5 |
| Monitor USB / capture | 0.8.1.4 / 0.8.1.30 |
| Desktop HUD | 0.8.0.51 |

## Improvements and test scope

Control preserves NV boot selection, UB3 reset interlock, cooperative USB/NV
save handling, IEC 8/9, C46 reset, optional WP override/feedback and RBCP.
The native C000 page plus decoded A000/E000 pages retain the 74LS138 mapping.
ROM and IEC Apply+Save were drive-tested while USB continuously powered OneROM.

Monitor uses the official plugin API and USB pins. Its passive core does not
start ROM-serving PIO, refuses leaving standby and restricts GPIO output to
board LEDs. Framed commands, bounded transmit buffering and diagnostics improve
USB transport. ABI31, PIO/DMA sampling, V51 telemetry and LED policy persist.

Monitor 0.8.1.30 invalidates stale RPM track-zone evidence after seeks and
capture gaps. RPM resumes only after a fresh valid physical header. Retesting
recorded 145 STATUS samples with R=0 Q=0, 113 RPM reports at 300.53–300.68 and
nine passing header checksums. No stale-zone 332–333 RPM spike was observed.

The public Monitor image uses default NV; the drive-tested image differed only
in preserved device settings. Public packaging additionally replaces two
absolute TinyUSB diagnostic filenames with equal-length portable strings.
Exactly 334 diagnostic-prefix bytes change; all other default-image bytes,
instructions, addresses and terminators are unchanged. The sanitizer is at
tools/maintenance/Sanitize-Monitor-Diagnostics.ps1. The private archive retains
the exact flashed image. The public stock Control image was recomposed
with tested core/plugins and the stock pages already distributed here. That
exact stock composition is build/inspection verified, not separately flashed.
The private drive ROM collection, device identifiers and recovery dumps stay
private. Public image hashes and sizes are in releases/v8.1-manifest.json.

## Build, flash and recovery

Use WSL/Linux, Arm GNU 15.3.Rel1, Make, current Rust/Cargo (Control wrapper
requires 1.93 or newer), and OneROM CLI. Build Control with
consolidation/scripts/build-control-v0.8.1.sh and owner-supplied lawful ROMs.
Build Monitor with firmware-monitor/Build-Monitor-Fire24G-V0.8.1.sh, setting
TOOLCHAIN and ONEROM_CLI. Its PowerShell wrapper accepts Linux paths via WSL.
Never interchange the role-specific core overlays or finished images.

Close HUD, identify the intended board explicitly, and back up before flashing.
Use the bundled CLI with --verify, rescan after reboot and inspect plugin
versions. See PUBLIC_FIRE24G_RELEASES.md for commands. A transient reboot
enumeration error requires a rescan and readback, not an assumption of failure.

Run tools/maintenance/Test-Public-Package.ps1 -PackageRoot . and
python -m unittest discover -s 1541hud/tests -v. Native Monitor tests cover
framing, queue bounds/backpressure and RPM validity. Hardware acceptance does
not certify every board revision or disk format.

## Consolidation decisions

The v8.1 branch starts from the public official-core upgrade and imports the
tested firmware source without private history. The older boot-selection fix
is superseded by Host-Control 0.8.1.5. Security branch history was reconciled;
the firmware baseline intentionally retains the exact tested upstream snapshot.
Host dependency refreshes require separate requalification. The older prototype
Dependabot lockfile candidate is not substituted for that snapshot.

The six older public branches were retired after their exact tips were
archived privately under archive/public-2026-11-09/. The old r1 release and
its original downloaded images are also backed up privately. The current
release replaces the old pinned Monitor firmware. Historical recovery images
remain private; this cleanup does not rewrite public Git history.
Automatic drive power-off sensing and the separate C64 configurator release
remain deferred. RBCP support stays present. RPM assumes standard DOS SYNC
counts; nonstandard disk formats need separate qualification.

Both complete source builds and the repeatable checker passed on 2026-11-09.
See [clean source-build verification](CLEAN_SOURCE_BUILD.md) for prerequisites,
commands, tool versions, results and the synthetic-image safety boundary.
