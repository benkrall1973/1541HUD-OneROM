# Host dependency audit update — November 17, 2026

The November 17 public CI run found three newly published RustSec advisories in the full workspace lock: RUSTSEC-2026-0523 and RUSTSEC-2026-0524 affected `quinn-proto` 0.11.18, and RUSTSEC-2026-0538 affected `quinn-udp` 0.5.15. The supported host build graph had no affected dependency, but the repository audit deliberately checks the full lockfile.

The tracked lock was updated to `quinn-proto` 0.11.19 and `quinn-udp` 0.5.16. No advisory was ignored and no workspace path was excluded. The supported source manifest records the new versions and lock checksum. The Windows CLI was rebuilt from that source and lock.

The follow-up GitHub Actions run is the verification record for the package checks, CLI build and audit. The original failed run is [#44015289553](https://github.com/benkrall1973/1541HUD-OneROM/actions/runs/44015289553). This note should be updated with the follow-up run link after it completes.
