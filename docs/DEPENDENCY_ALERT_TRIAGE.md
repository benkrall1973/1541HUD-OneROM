# Dependency-alert triage

The active source package includes the supported host security fixes. The complete original upstream snapshot is recoverable from the historical tag/private backup; unsupported Studio and Studio v2 prototype build trees are excluded from the current package. This fixes or removes affected dependency paths rather than dismissing alerts.

## Pre-cleanup GitHub inventory

| Records | Dependency and paths | Resolution in current source |
| --- | --- | --- |
| 19 | ZIP in firmware/CLI manifests and workspace/prototype locks | Checked-in ZIP 9.5.0; excluded prototype tree removed |
| 8 | Rustls in workspace/prototype locks | Checked-in Rustls 0.23.46; excluded prototype tree removed |
| 1 | LRU through Studio's graphics stack | Studio excluded from workspace and package; LRU absent from pruned lock |

The four critical ZIP entries describe the same advisory in four dependency files. The live inventory and source update are documented in [the cleanup record](SOURCE_SECURITY_CLEANUP_2026-11-10.md). Confirm GitHub has rescanned the new default-branch commit before reporting its open-alert count as cleared.

## Validation and scope

The active lock was pruned without adding or changing retained package versions at the time of the November 10 cleanup. On November 12, a rustix unsoundness advisory affected the pinned 1.1.4; the supported lock and bundled Windows CLI were updated to patched rustix 1.1.6. CI run [#52](https://github.com/benkrall1973/1541HUD-OneROM/actions/runs/43277779018) passed all five jobs at that point. On November 24, RUSTSEC-2026-0592 affected `url` 2.5.8, so the supported lock was advanced to `url` 2.5.9 and `form_urlencoded` 1.2.3. The audit scans the complete lock, reports all findings, and fails on vulnerabilities, yanked packages, or unsoundness in the supported CLI normal/build graph. Findings outside that graph remain visible in the report but do not fail this host-only gate. No advisory ignore list is used. Maintenance warnings are not a guarantee of future support.

The RP2350 firmware does not embed the Rust ZIP/TLS libraries. That separation does not justify ignoring an affected build tool or distributing a vulnerable host executable.

Use the current supported source and documented wrappers, trusted ROM/configuration inputs and verified image hashes. A historical upstream tag is provenance, not a supported host build. Patching archive libraries does not impose resource limits on arbitrary untrusted archives.

## Before closing any future alert

1. Record the advisory, affected version and dependency path.
2. Determine whether it affects firmware runtime, supported host builds, or excluded material.
3. Update the actual active manifests/lockfile or remove an unused unsupported build tree while preserving its provenance.
4. Run the dependency audit, source verification, relevant regressions and full role builds; confirm the advisory is absent from the supported target graph.
5. Confirm GitHub's refreshed dependency graph and keep historical release status explicit.

See [host-source security policy](HOST_DEPENDENCY_OVERLAY.md) and [source provenance](../source/one-rom-v0.8.1/1541HUD-SOURCE.json).
