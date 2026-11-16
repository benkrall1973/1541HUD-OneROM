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
retained package version. The pinned upstream OneROM core C source remains
unchanged; project-authored Control USB plugin source is maintained separately.

The original complete snapshot remains recoverable from project tag
`v8.1-maintenance-2026-11-10` and the private recovery archive. The
[source provenance manifest](source/one-rom-v0.8.1/1541HUD-SOURCE.json) records
its tree hashes, changed host files and exclusions. Project-authored Control
USB plugin source is maintained separately under `firmware-control/`.
Preserving a historical snapshot does not make its old host tools a supported
build path.
The custom integration and patched host CLI are community builds, not official
upstream binary distributions. Upstream software/firmware and documentation
are licensed under the upstream MIT license; hardware material has its own
CERN-OHL-W-2.0 terms.
See `source/one-rom-v0.8.1/LICENSE.md` and the upstream project at
<https://github.com/piersfinlayson/one-rom>.

## Other dependencies

Vendored dependencies retain their own licenses and copyright notices. Their
license files must remain with any redistribution of the relevant source.

## ROM library

The curated `roms/` folder contains stock Commodore DOS 2.6 drive pages
and C64 KERNAL, SpeedDOS+ profile pages and host KERNAL, and DolphinDOS 2
source, license, reference image, pages, and host KERNAL. Exact sizes and
SHA-256 values are recorded in `roms/ASSETS.sha256`; the public
package validator enforces the binary allowlist and hashes. DolphinDOS source
retains the upstream Unlicense. The SpeedDOS+ files are included as the
specific profile assets accepted for this project; their exact build is not
reproducible from source in this repository. See the
[package boundary](docs/PUBLIC_PACKAGE_MANIFEST.md).

The package also includes three Fire-24G firmware images in `releases/`: the
UB3 V51 stock baseline, the UB3 V53 Apply Only candidate, and the UB4 passive
Monitor image. The Monitor composition inputs include the OneROM Universal
bootloader and the stock C000 page. See
[release contents](docs/PUBLIC_FIRE24G_RELEASES.md).

The root MIT license covers project-authored integration materials; it does
not grant rights to the Commodore ROM payload or other third-party material.
Inclusion in the baseline and hash verification are not a separate license
grant. The upstream bootloader and other dependencies retain their applicable
upstream notices.

JiffyDOS is licensed commercial software. No JiffyDOS drive image, C64 KERNAL,
extracted page, S-JiffyDOS generated image/page, or composed JiffyDOS image is
included. The `jiffydos/` and `s-jiffydos/` folders contain documentation and
project-authored patch/build source only. Builders must supply licensed
JiffyDOS inputs. The root MIT license does not cover third-party ROM contents.

