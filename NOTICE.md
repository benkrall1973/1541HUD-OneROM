# Notices and provenance

## 1541HUD / OneROM Fire-24G integration

This repository contains project-authored integration code, documentation,
build wrappers, and ROM-free templates for a Commodore 1541 installation using
OneROM Fire-24G hardware. Those project-authored materials are licensed under
the root [MIT License](LICENSE).

## OneROM upstream snapshot

`source/one-rom-v0.8.1/` is the supported source derived from Piers Finlayson's
official OneROM v0.8.1 release source at commit
`1b189260d85a5feced19e969399986f5f1d1cabe` on `releases/0.8.1`.
The tested host dependency fixes are now checked in. Unsupported upstream
Studio and Studio v2 prototype build trees are excluded from the active
package; the remaining lockfile was pruned without adding or changing any
retained package version. Firmware C source remains unchanged.

The original complete snapshot remains recoverable from project tag
`v8.1-maintenance-2026-11-10` and the private recovery archive. The
[source provenance manifest](source/one-rom-v0.8.1/1541HUD-SOURCE.json) records
its tree hashes, changed host files and exclusions. Preserving a historical
snapshot does not make its old host tools a supported build path.
The custom integration and patched host CLI are community builds, not official
upstream binary distributions. Its software/firmware and documentation are licensed under the
upstream MIT license; its hardware material has its own CERN-OHL-W-2.0 terms.
See `source/one-rom-v0.8.1/LICENSE.md` and the upstream project at
<https://github.com/piersfinlayson/one-rom>.

## Other dependencies

Vendored dependencies retain their own licenses and copyright notices. Their
license files must remain with any redistribution of the relevant source.

## Stock baseline inputs and excluded ROM collections

The public package deliberately includes two named stock firmware images in
`releases/`. The Control stock image contains Commodore 1541 DOS 2.6. The
Monitor composition inputs in `firmware-monitor/inputs/` include the stock
C000 page `1541-c000.325302-01.bin` and the OneROM Universal bootloader.
The package validator checks these named inputs and images against recorded
hashes. See [release contents](docs/PUBLIC_FIRE24G_RELEASES.md) and the
[package boundary](docs/PUBLIC_PACKAGE_MANIFEST.md).

The root MIT license covers project-authored integration materials; it does
not grant rights to the Commodore ROM payload or other third-party material.
Inclusion in the baseline and hash verification are not a separate license
grant. The upstream bootloader and other dependencies retain their applicable
upstream notices.

Optional JiffyDOS, SpeedDOS, DolphinDOS and other private DOS collections,
their ROM-derived pages, and custom composed images are excluded. Builders
must supply ROM pages they are entitled to use. A template or documentation
reference does not grant permission to copy or redistribute those ROMs.

