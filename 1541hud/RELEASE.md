# 1541HUD Release Process

This file documents 1541HUD releases. The repository root `RELEASE.md` is retained as upstream OneROM reference material.

## Stable release rules

- `v0.0.30` is the immutable hardware-proven DriveHUD baseline.
- `v0.0.31` is the current stable 1541HUD rename baseline.
- Released tags are never moved or rewritten.
- Generated BIN/UF2 files are build products and are not committed to the repository.
- Runtime changes require a new version. Documentation-only cleanup does not.

## Before tagging a new 1541HUD release

1. Update `1541hud/CHANGELOG.md` with the exact tag version.
2. Update source/build filenames and displayed version strings only when the runtime version changes.
3. Run the canonical PowerShell build and record its output/hashes for the release notes or handoff.
4. Confirm `1541HUD Sanity` passes.
5. Confirm any relevant OneROM base CI checks pass.
6. Verify the image on real Commodore 1541 hardware when the release changes acquisition, decoding, mailbox behavior, or USB transport.
7. Tag the exact tested commit and push the tag.

The `.github/workflows/release.yml` workflow creates the GitHub release from the matching section in `1541hud/CHANGELOG.md`.

## Upstream OneROM

1541HUD is based on OneROM v0.7.1. Upstream updates should be reviewed and integrated deliberately. Do not automatically merge upstream changes into a stable 1541HUD release line.
