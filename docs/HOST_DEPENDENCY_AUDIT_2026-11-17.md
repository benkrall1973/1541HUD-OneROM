# Host dependency audit update — November 17, 2026

The November 17 public CI run found four newly published RustSec advisories in the full workspace lock. RUSTSEC-2026-0523 and RUSTSEC-2026-0524 affected `quinn-proto` 0.11.18; RUSTSEC-2026-0538 affected `quinn-udp` 0.5.15; and RUSTSEC-2026-0544 affected `libz-sys` 1.1.31 through 1.1.32. RustSec lists `libz-sys` 1.1.33 and later as patched. The supported host build graph had no affected dependency, but the repository audit deliberately checks the full lockfile.

The tracked lock was updated to `quinn-proto` 0.11.19, `quinn-udp` 0.5.16, and `libz-sys` 1.1.33. No advisory was ignored and no workspace path was excluded. The supported source manifest records the patched versions and lock checksum. The Windows CLI must be rebuilt from this final source and lock.

The first follow-up run [#44112789895](https://github.com/benkrall1973/1541HUD-OneROM/actions/runs/44112789895) confirmed the ARM builds, firmware checks, and Windows CLI tests passed, but its audit caught RUSTSEC-2026-0544 and its package check rejected the stale bundled CLI provenance. A second run must verify the `libz-sys` update and rebuilt CLI package before this audit is considered complete.
