# Third-party DOS page preparation

The public ROM library includes selected stock DOS 2.6, SpeedDOS+, and
DolphinDOS 2.0 profile pages. This guide covers additional inputs, including
licensed JiffyDOS files supplied by the builder. It does not include JiffyDOS
ROM data or generated S-JiffyDOS pages.

`utilities/rom-pages/Split-1541-RomPages.ps1` writes exact 8192-byte pages and
prints SHA-256 hashes. It never downloads ROMs.

For a conventional 16 KiB C/E source image:

```powershell
.\utilities\rom-pages\Split-1541-RomPages.ps1 -InputPath .\private-roms\source.bin -OutputDirectory .\private-roms\control -PageOrder CE
```

For a verified 24 KiB image already ordered C/A/E:

```powershell
.\utilities\rom-pages\Split-1541-RomPages.ps1 -InputPath .\private-roms\source.bin -OutputDirectory .\private-roms\control -PageOrder CAE
```

For the supplied SpeedDOS+ 16 KiB C/E image, follow the exact command and
page mapping in [`speeddos-plus/README.md`](rom-library/speeddos-plus/README.md).
That profile's upper page is also used for A, so `-MirrorEToA` is appropriate
for that specific input. Do not assume the flag is valid for another DOS.

For DolphinDOS 2.0, use the already prepared and hardware-tested C/A/E pages
in [`dolphindos-2/drive-pages/`](rom-library/dolphindos-2/README.md). The
separate 32 KiB `dd2_1541.rom` reference image is not byte-identical to those
pages; this general splitter is not a substitute for identifying the exact
source image and memory map first.

`-MirrorEToA` is only for a DOS whose documented A page is an E-page mirror.
Do not use it for DolphinDOS 2 or any ROM whose A page is distinct. If the
source size/order is unknown, stop and verify it; a neatly split wrong image is
still wrong.
