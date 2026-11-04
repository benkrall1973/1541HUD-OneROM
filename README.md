# 1541HUD / OneROM Fire-24G V8.1

ROM-free source package for the paired Fire-24G 1541HUD system. It includes
the upstream **OneROM CLI 0.5.0** at `tools/onerom-v0.5.0.exe`, so a Windows
user can scan and flash the supplied test images without first downloading a
separate tool.

## Quick start: flash the supplied Fire-24G images

1. Connect only the intended OneROM and put it into BOOTSEL.
2. From PowerShell, run `./tools/onerom-v0.5.0.exe --unrecognised --board fire-24-g scan`.
3. Confirm the displayed serial belongs to the board you intend to program.
4. Flash **one** of the images using that serial:

   `./tools/onerom-v0.5.0.exe --serial <SERIAL> program --firmware ./releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-test-v0.8.1.bin --verify`

   or:

   `./tools/onerom-v0.5.0.exe --serial <SERIAL> program --firmware ./releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.28.bin --verify`

The UB3 Control and UB4 Monitor images are not interchangeable. Read
`docs/PUBLIC_FIRE24G_RELEASES.md` before flashing.

For custom drive DOS images, supply your own lawful ROM inputs under
`private-roms/`, then follow `docs/UB3_CONTROL_CLI_COMPOSITION_GUIDE.md`.
Start technical/background reading with
`docs/FIRE24G_V81_ENGINEERING_DOSSIER.md`.

Run `tools/maintenance/Test-Public-Package.ps1 -PackageRoot .` before
initializing or publishing the resulting repository.
