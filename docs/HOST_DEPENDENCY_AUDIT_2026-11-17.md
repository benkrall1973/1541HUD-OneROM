# Host dependency audit update — November 17, 2026

The public CI audit found four RustSec advisories in the full workspace lock. RUSTSEC-2026-0523 and RUSTSEC-2026-0524 affected `quinn-proto` 0.11.18; RUSTSEC-2026-0538 affected `quinn-udp` 0.5.15; RUSTSEC-2026-0544 affected `libz-sys` 1.1.31 and 1.1.32. The supported host build graph is audited alongside the full lockfile.

The lockfile now pins patched `quinn-proto` 0.11.19, `quinn-udp` 0.5.16, and `libz-sys` 1.1.33. No advisory was ignored and no workspace path was excluded. The source manifest and checked-in lock checksum were updated together.

CI run [#44117179224](https://github.com/benkrall1973/1541HUD-OneROM/actions/runs/44117179224) passed the RustSec audit, supported-source provenance check, CLI/ZIP regressions, firmware checks, ARM builds, and Windows CLI build. The package job in that run rejected the older bundled CLI files, as expected while replacing them. The Windows CLI build also exposed a hash-format mismatch: Windows expanded text files to CRLF while the package validator compares canonical LF hashes. The build script now records normalized LF hashes, and the checked-in executable, dependency inventory, licenses, and manifests were refreshed from the passing CI artifact.

The public workflow run triggered by the bundle update is the final package-boundary and HUD regression verification. It builds firmware and CLI artifacts only; it does not program hardware.
