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

`-MirrorEToA` is only for a DOS whose documented A page is an E-page mirror.
Do not use it for DolphinDOS 2 or any ROM whose A page is distinct. If the
source size/order is unknown, stop and verify it; a neatly split wrong image is
still wrong.
