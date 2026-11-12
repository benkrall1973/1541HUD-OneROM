# Supported-source security cleanup

The preceding release applied tested host security fixes only after exporting
an unchanged upstream snapshot. GitHub therefore continued to report real
vulnerabilities against the older files in that snapshot. This cleanup makes
the active source reflect the supported build, without dismissing alerts.

## Changes and provenance

- Check in the existing tested ZIP 9.5.0, Rustls 0.23.46 and associated host
  dependency updates, including locked metadata/config-generator builds.
- Exclude the unsupported `rust/studio` and `prototypes/studiov2` trees from
  the active public source; remove Studio from workspace membership.
- Relocate the CLI's shared Windows icon from Studio into `rust/cli/assets`
  and update the CLI build-script reference. The icon bytes are unchanged;
  this preserves Windows resource compilation without restoring the GUI tree.
- Prune the workspace lockfile with `cargo update --workspace --offline`.
  Its package count changes from 901 to 527. No package/version was added;
  all 527 retained package versions match the previously tested overlaid lock.
  The Studio-only LRU dependency is absent.
- Verify the supported-source files and dependency pins with
  `tools/maintenance/Verify-Source-Dependencies.py`. Build wrappers verify
  current source instead of reapplying a patch that is already incorporated.
- Fail the workspace audit on any unsoundness advisory, not only those in
  the CLI graph. Maintenance-status warnings remain reported.

The [source manifest](../source/one-rom-v0.8.1/1541HUD-SOURCE.json) records the
upstream revision, original tree hashes, changed-file checksums, dependency
versions and exclusions. The complete original source can be recovered from
project tag `v8.1-maintenance-2026-11-10` or the private recovery bundle.
Historical source is retained as provenance, not offered as a supported build.

## Alert inventory and checks

The live pre-cleanup inventory contained 28 alerts: 19 ZIP records across
workspace/prototype manifests and locks, eight Rustls records across the two
locks, and one Studio LRU record. The four critical entries were the same ZIP
advisory in four dependency files. The dependency versions and active paths
are corrected or removed here; no alert is bulk-dismissed or reclassified as
a false positive.

The local RustSec scan at database commit
`28d73fc95e5972af3403225a217def57dcaba3c4` found zero vulnerability records,
zero unsoundness records and zero yanked packages. Four unmaintained-package
warnings remain: bare-metal, basic-toml, paste and proc-macro-error2, outside
the supported CLI normal/build graph. These are not treated as vulnerability
records or silently suppressed.

Full CI builds both firmware roles and their custom plugins, runs Linux and
Windows CLI/archive regressions and audits the active lockfile. All five CI
jobs passed for commit `b89524acfcf15ca13fc343584e8655871eb9497b`.
On 2026-11-10, GitHub independently confirmed all 28 alerts as fixed, with
zero open alerts and zero dismissals. The source tag
[`v8.1-source-security-2026-11-10-r1`](https://github.com/benkrall1973/1541HUD-OneROM/tree/v8.1-source-security-2026-11-10-r1)
preserves the code snapshot. The superseded Release post and its attachments
were removed during release cleanup; the machine-readable alert inventory and
audit summary remain in `qualification/source-security-cleanup-2026-11-10.json`.
This records the status at verification time; future advisories can change it.

## Release and hardware boundary

Firmware C source, role overlays, custom plugin code and the published Control
and Monitor image bytes are unchanged. The supplied Windows CLI 0.6.0-hud1
was already built with patched host dependencies and is retained, with its
original lock/overlay provenance. New source-built CLI candidates additionally
record the supported-source manifest hash. No firmware flash is needed for
this source/package cleanup.

The exact public stock two-ROM Control image passed the documented drive test
on 2026-11-10. Full USB disconnect/reconnect persistence was not reported.
The C64 configurator and physical prototype-PCB qualification remain outside
the current public release scope.
