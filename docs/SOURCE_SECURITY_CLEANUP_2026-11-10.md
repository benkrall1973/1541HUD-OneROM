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
Windows CLI/archive regressions and audits the active lockfile. GitHub's
dependency graph must process the new default-branch commit before alert
closure can be confirmed; CI success alone does not prove the dashboard has
rescanned.

## Release and hardware boundary

Firmware C source, role overlays, custom plugin code and the published Control
and Monitor image bytes are unchanged. The supplied Windows CLI 0.6.0-hud1
was already built with patched host dependencies and is retained, with its
original lock/overlay provenance. New source-built CLI candidates additionally
record the supported-source manifest hash. No firmware flash is needed for
this source/package cleanup.

Stock Control drive/full-power-cycle qualification, C64 configurator work,
and physical prototype-PCB qualification remain separate outstanding work.
