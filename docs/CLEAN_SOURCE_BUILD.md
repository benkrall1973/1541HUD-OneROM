# Clean source-build verification

Use a Git checkout of the v8.1 branch in WSL or Linux. Install Arm GNU
15.3.Rel1, Rust/Cargo, GNU Make, Git, patch, tar, Python 3 and native C/C++
build tools. The Control wrapper checks for Cargo 1.93 or newer; the recorded
clean-build environment used Cargo 1.99.0. Keep the selected Rust toolchain on
PATH. Network access is needed for pinned upstream sources and Cargo crates.

Run both complete firmware builds without supplying a private ROM collection:

```sh
export TOOLCHAIN=/path/to/arm-toolchain/bin
bash tools/maintenance/Test-Source-Build.sh
```

This builds the official core, custom Control plugins, host CLI, passive
Monitor core and custom Monitor plugins, then composes and inspects both
images. It uses the CLI built from the checkout to compose Monitor. The
Control test pages are synthetic; its image must never be installed in a
drive. Outputs are ignored under .buildtools/source-check. No command in this
check scans, programs or reboots a device.

For a usable Control image, supply lawful ROM pages and the appropriate
template described in UB3_CONTROL_CLI_COMPOSITION_GUIDE.md. The Monitor build
uses its tracked baseline inputs. Installed release images and their test
scope remain documented in V81_BASELINE_RUNBOOK.md and the release manifest.

The wrappers re-export tracked official source into separate ignored work
trees before applying role-specific overlays. Both wrappers also apply the checksummed host-tool
dependency overlay described in HOST_DEPENDENCY_OVERLAY.md. The vendored
official source stays unchanged; the exported Rust builds use --locked.
Dependency/compiler caches may be reused on later runs; a fresh clone establishes
the initial source build.
Build date, Git metadata and diagnostic filenames can change image hashes.
New builds do not replace the archived hardware-tested images automatically.

Before publishing a package, validate a clean checkout or export using
tools/maintenance/Test-Public-Package.ps1 -PackageRoot . . Run that check
before compiling, since generated firmware and private build manifests are
intentionally excluded from the public package. Run HUD tests with
python -m unittest discover -s 1541hud/tests -v. The public CI also runs native
Monitor protocol/RPM and Control cooperative-wait regression tests.

## Recorded verification: 2026-11-09

Both roles were built from a fresh public v8.1 clone with Arm GNU 15.3.Rel1
and Cargo 1.99.0. The repeatable checker and wrapper cleanup at 10fd3450 were
then applied and the combined checker passed using its source-built CLI 0.6.0.
Downloaded dependencies were fetched at the official pins; no local build kit
or private DOS collection was copied into the checkout.

| Build-check output | Bytes | SHA-256 |
| --- | --- | --- |
| Synthetic Control; never flash | 262144 | 362A95537AF5D0BAA1EF8572F549E02F9C7B24F72D23980D96183416DD999BC2 |
| Monitor verification build | 229376 | D5F9232C4D2EC24AFA55E02A9386438214E182A9DAFBF546C4606D7B30312E09 |

These hashes identify this source-build check, not the published release
images. Composition/inspection confirmed core v0.8.1, Control USB .16 / Host
.5, Monitor USB .4 / capture .30, and Monitor standby enabled. No device was
programmed. All 48 HUD tests, native protocol/RPM/cooperative-wait tests,
public-package checks and local documentation-link checks also passed.
