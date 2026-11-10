# Host-tool dependency security overlay

The official OneROM v0.8.1 tree under source/one-rom-v0.8.1 remains unchanged.
Supported Control and Monitor wrappers export it into disposable build trees,
then apply overlays/host-dependencies-v0.8.1.patch before compiling Rust tools.
The role-specific C firmware overlays are applied separately.

The host overlay updates the ROM-fetching library and CLI to ZIP 9.5.0 and
locks Rustls 0.23.46. Compatible lockfile updates also select ctutils 0.4.4,
libz-sys 1.1.31, webpki-roots 1.0.10 and quinn-proto 0.11.18. The accompanying
JSON records exact versions and SHA-256 hashes of the patch and resulting lock.
Version selection follows the [ZIP release](https://github.com/zip-rs/zip2/releases/tag/v9.5.0)
and the [RustSec Rustls advisories](https://rustsec.org/advisories/RUSTSEC-2026-0346).
The helper verifies them after applying the patch with zero fuzz. Rust build
commands use --locked so dependency resolution cannot change silently.

## Scope

These dependencies run on the build/CLI host. They are not libraries in the
RP2350 C firmware. Published firmware images, ROM collections, NV contents,
GPIO wiring and plugin versions are not replaced by this change. No board
needs flashing to update the source-built host tools.

Use the supported wrappers or the combined Test-Source-Build.sh checker to
obtain the patched tools. Building directly in the pristine upstream directory
bypasses the overlay. The bundled Windows onerom-v0.5.0.exe is a legacy binary
and is not rebuilt or covered by this overlay's validation. Use the freshly
built CLI for archive retrieval/composition; do not treat the old executable
as a patched host-tool distribution.

## Validation commands

After the Control wrapper has prepared its core tree, set core to
firmware-control/.work/control-core-v0.8.1/one-rom-v0.8.1 and run:

```sh
bash tools/maintenance/Test-Host-Dependencies.sh "$PWD/$core"
cargo install cargo-audit --version 0.22.2 --locked
bash tools/maintenance/Audit-Host-Dependencies.sh "$PWD/$core"
```

The test helper uses actual onerom-fw archive loading with synthetic stored
and deflated members, cache/no-cache behavior, missing members, truncated
archives, invalid ZIP bytes and damaged CRCs. It also runs CLI unit tests.
Image conversion/transformation and general CLI integration tests also run.
USB-discovery suites run only when the Linux USB sysfs directory exists;
otherwise the helper prints an explicit skip. The broad upstream firmware
download suite is outside this focused check. Generated fixtures and added
integration tests stay in the disposable tree.
Public CI repeats these checks against a fresh source export.

The audit helper scans the whole prepared workspace, then compares findings
against the all-target normal/build dependency graph of onerom-cli and
onerom-metadata. Vulnerabilities in that supported graph fail the check. Other
workspace findings remain printed and recorded; they are not silently ignored.
Advisory warnings are reported separately. A reused advisory database is a
snapshot; use a fresh directory for a fresh database fetch.

## Remaining boundaries

The pristine vendored lockfile still describes older dependencies. GitHub may
continue reporting alerts against that stored snapshot even when the supported
build applies the overlay. This change does not dismiss those alerts.

The prepared-workspace scan still reports Quick-XML 0.39.4 advisories
RUSTSEC-2026-0194 and RUSTSEC-2026-0195. Quick-XML is outside the supported
CLI/metadata dependency graph. Updating the separate tooling that uses it
requires its own compatibility work; a workspace-wide clean audit is not
claimed. Continue using trusted ROM/configuration inputs: patched dependencies
do not provide resource limits for arbitrary archives.

## Recorded results: 2026-11-09

Both complete firmware builds, composition and inspection passed with the
final overlay using Cargo 1.99.0 and Arm GNU 15.3.Rel1. Host tests passed 573
checks with eight upstream tests ignored. USB-discovery suites were explicitly
skipped because this WSL environment has no USB sysfs directory. All 48 HUD
tests and public-package validation passed. No board was programmed.

Cargo-audit 0.22.2 with advisory database commit
d9cb059f3217db87aeccd190b81eaffe8cc29cb9 found 27 vulnerability records in the
pristine workspace lock and two in the overlaid lock. The two remaining
Quick-XML records are outside the supported host graph; that graph has zero
vulnerability records in this snapshot. Unmaintained/unsound/yanked advisory
warnings are still reported. These counts are advisory records, not a claim
about exploitability of the installed drive firmware.

Exact versions, patch/lock hashes and test scope are recorded in
host-dependency-validation.json. Published firmware and the legacy Windows
executable are unchanged; no release-image or Windows-binary replacement is
claimed by these source-build results.
