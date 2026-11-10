# Host-tool dependency security overlay

The official OneROM v0.8.1 source remains unchanged at
source/one-rom-v0.8.1. Supported builds export it into disposable work trees
and apply overlays/host-dependencies-v0.8.1.patch. Role-specific C firmware
overlays are separate. Every patched build checks the overlay and lockfile
hashes against overlays/host-dependencies-v0.8.1.json and uses Cargo --locked.

## Current dependency resolution

The overlay selects ZIP 9.5.0, Rustls 0.23.46, ctutils 0.4.4, libz-sys 1.1.31,
webpki-roots 1.0.10, quinn-proto 0.11.18, Quick-XML 0.41.0,
wayland-scanner 0.31.11, Git2 0.21.0, built 0.8.1, Tokio 1.54.1,
event-listener 5.4.2, async-executor 1.14.1, chacha20 0.10.2 and
clipboard-win 5.4.3. CLI and Studio build dependencies both select Git2 0.21;
this avoids retaining an older Git2 through feature unification.

Quick-XML 0.41 fixes
[RUSTSEC-2026-0194](https://rustsec.org/advisories/RUSTSEC-2026-0194.html) and
[RUSTSEC-2026-0195](https://rustsec.org/advisories/RUSTSEC-2026-0195.html).
The updated Wayland scanner supports that parser. CI compiles its actual
wayland-client and wayland-protocols consumers.

These are host dependencies. They are not libraries in the RP2350 C firmware.
The drive-tested Control and Monitor image bytes, plugin versions, ROM
collection and NV settings are unchanged. Updating the CLI needs no flashing.

## Windows distribution

The supported Windows executable is tools/onerom-v0.6.0-hud1.exe, built from
the pinned upstream CLI 0.6.0 with this overlay. hud1 identifies this community
distribution; --version continues to report upstream 0.6.0. It is unsigned.
See [Windows CLI build and use](WINDOWS_CLI.md), tools/windows-cli-manifest.json,
and the accompanying dependency inventory and license notices.

The prior onerom-v0.5.0.exe is retired from the current tree. Historical
commits/releases still retain it. A direct build in the pristine upstream
directory bypasses the overlay; use the supported scripts.

## Validation

Linux/WSL host checks:

```sh
bash tools/maintenance/Test-Host-Dependencies.sh /path/to/prepared-core
cargo install cargo-audit --version 0.22.2 --locked
bash tools/maintenance/Audit-Host-Dependencies.sh /path/to/prepared-core
```

The ZIP regressions exercise actual onerom-fw archive loading with synthetic
stored/deflated members, missing members, invalid/truncated ZIPs and CRC
damage. CLI unit, image conversion/transformation and general integration
tests run. USB-discovery suites require USB sysfs on Linux; unavailable sysfs
is reported as an explicit skip. Windows CI runs the native argument and scan
regressions against the tests' nonexistent VID/PID, not an attached board.

Windows builds and release-profile tests use Build-Windows-CLI.ps1.
Full ARM CI uses Test-Source-Build.sh to compile the official cores and all
four custom plugins, compose both images, and inspect them with the
source-built CLI. Its Control image contains synthetic pages and must never
be flashed. CI publishes logs and hash records, not the synthetic images.

## Audit policy and remaining boundaries

The audit now fails on any vulnerability record in the complete prepared
workspace lockfile, any yanked package, or an unsoundness advisory in the
supported CLI/metadata normal/build dependency graph across all targets.
Warnings remain printed with their scope; there is no advisory ignore list.

The 2026-11-10 fresh database scan found zero vulnerability records and zero
yanked packages. No unsoundness advisory remains in the supported host graph.
Two LRU unsoundness records remain in the unused upstream Studio graphics
chain (iced_glyphon -> iced_wgpu -> Studio); neither CLI nor firmware uses it.
Unmaintained-package warnings also remain in upstream GUI, prototype and
development dependencies. Replacing that GUI stack is separate work; this
integration does not build or distribute Studio.

The pristine vendored source and its older lockfiles remain preserved for
provenance. GitHub can therefore continue reporting alerts against them,
including the excluded Studio prototype. A clean supported build does not
mean those historical source alerts have vanished. Use trusted ROM/config
inputs: patched archive libraries do not impose resource limits on arbitrary
archives.

The previous 2026-11-09 validation is historical. Its hashes and counts in
host-dependency-validation.json describe the earlier overlay, not hud1.
Current evidence is recorded in qualification/host-maintenance-2026-11-10.json
and the linked CI run.
