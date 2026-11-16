# DolphinDOS 2.0

DolphinDOS 2 is a separate system with its own 1541 drive ROM and C64-side
KERNAL. It does **not** use JiffyDOS as its base and does not need a JiffyDOS
patch. The upstream project has separate `1541.asm` and `kernal.asm` sources
and documents the build in its [source repository](https://github.com/donnchawp/DolphinDOS2).

For the high-speed path, use the matching DolphinDOS C64 KERNAL, DolphinDOS
drive ROM, parallel cable, and the required drive-side RAM configuration.
With the ordinary IEC cable alone, do not call it a tested parallel-speed
setup.

## Included files and build source

This folder includes the upstream source, its accompanying Unlicense, the
upstream reference drive image, the extracted OneROM drive pages, and the
matching C64 KERNAL used in this project. The source files and license are
from the [DolphinDOS 2 upstream repository](https://github.com/donnchawp/DolphinDOS2).
The extracted pages and KERNAL are 8192 bytes each; sizes and SHA-256 values
are recorded in [the library asset manifest](../ASSETS.sha256).

The reference image is supplied for source/build comparison; the three
`drive-pages/` files are the OneROM C/A/E inputs. The page order is deliberate:
do not mirror E into A. Retain the included license when reusing the upstream
source. These files do not claim that a parallel transfer was qualified on
every board revision or cable installation.
