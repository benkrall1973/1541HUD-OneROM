# Supported host-source dependency security

The active source at `source/one-rom-v0.8.1` already contains the tested host security fixes. It is derived from the pinned official v0.8.1 revision, not an unchanged copy of the complete upstream workspace. Firmware C source remains unchanged.

## Current build path

Supported wrappers export the tracked source and call `Apply-Host-Dependencies.sh`. For a current export, that helper verifies `1541HUD-SOURCE.json`, changed-file checksums, dependency versions and exclusion of unsupported GUI trees; it does not apply the security patch twice. All Rust builds use `--locked`.

The active lock retains ZIP 9.5.0, Rustls 0.23.46, Git2 0.21.0, built 0.8.1 and the previously tested compatible host dependencies. Removing Studio pruned 374 GUI-only packages without changing any of the 527 retained package versions. There is no LRU, Quick-XML or Wayland dependency in the current lock.

`overlays/host-dependencies-v0.8.1.patch` and its JSON manifest are retained as historical provenance for the supplied Windows CLI and older pristine exports. The helper still supports such older exports. They do not describe the new pruned workspace lock. See the [current source manifest](../source/one-rom-v0.8.1/1541HUD-SOURCE.json) for current checksums.

## Supplied Windows CLI

The supported executable remains `tools/onerom-v0.6.0-hud1.exe`. It was already built using the patched dependency resolution and retains its original binary, overlay and lock hashes in `tools/windows-cli-manifest.json`. This source cleanup does not replace it or require firmware flashing. New source-built candidates additionally record `source_hardening_sha256` and the current lock hash.

A direct build of the current checked-in supported source no longer bypasses the host security fixes. Historical pristine source can be recovered from project tag `v8.1-maintenance-2026-11-10` and private backups; it is not a supported alternative host build. Do not build the excluded upstream Studio/prototype snapshot using this project's qualification claims.

## Validation and audit policy

```sh
python3 tools/maintenance/Verify-Source-Dependencies.py /path/to/prepared-core
bash tools/maintenance/Test-Host-Dependencies.sh /path/to/prepared-core
bash tools/maintenance/Audit-Host-Dependencies.sh /path/to/prepared-core
```

The audit fails on any active workspace vulnerability, yanked package or unsoundness record; there is no advisory ignore list. Maintenance-status warnings are printed and classified. The local post-cleanup scan has zero vulnerability, unsoundness and yanked records; four unmaintained-package warnings remain outside the supported CLI normal/build graph.

CI runs native Linux/Windows CLI and archive regressions, full ARM builds of both roles and all four custom plugins, and package/HUD/source checks. Synthetic Control test images are never published as flashable artifacts. Source builds do not replace the drive-accepted images automatically.

See the [source cleanup record](SOURCE_SECURITY_CLEANUP_2026-11-10.md), [Windows CLI guide](WINDOWS_CLI.md), and [dependency alert classification](DEPENDENCY_ALERT_TRIAGE.md).
