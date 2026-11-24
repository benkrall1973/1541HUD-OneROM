# 1541HUD Windows application

V0.8.0.58 is the current public HUD version and has been confirmed working on
the owner's configured drive setup. V58 includes the V53 runtime-only
IEC address behavior and UI updates; Apply Only requires UB3 Control USB
plugin 0.8.1.17 or newer. The current public source is 0.8.1.18. The HUD version is independent of OneROM firmware
v0.8.1.

The startup ROM picker shows profile names without slot numbers while
preserving the underlying slot index for Save and Apply. The ROM image defines
the list order. A label beginning with `@hidden:` is omitted from the picker
without renumbering the profile. The picker and other menus use taller,
wrapping touch rows with scrolling arrows; ROM help explains the profile and
matching C64 KERNAL. The V58 help card returns to the ROM list when tapped.
Appearance swatches and ROM help buttons use the regular button styling. The
Monitor dashboard keeps longer ROM names visible, and the UB4 LED card reports
unassigned/disconnected states and missing `HUDLED?` replies clearly. These
changes are in the HUD only; the OneROM firmware and stored ROM profiles are
unchanged.

Apply Only changes the temporary IEC address through `ROMIEC_NOW` without
writing NV2. The temporary address remains while UB3 stays powered. If a
1541-only power cycle must also restart UB3 and reload the saved address while
USB data stays connected, use a data-capable cable with host VBUS +5 V blocked,
or power-cycle UB3 explicitly. USB host +5 V at the Fire-24G USB port is a
normal UB3 power source; the VBUS note describes saved-address behavior, not a
need to remove USB power during ordinary use. The older v0.8.1.16 stock image
is the V51 baseline; Apply Only needs USB 0.8.1.17 or newer.

The public package includes Python source, launchers, and a standalone Windows
x64 executable at
[`releases/v0.8.0.58/1541HUD-V0.8.0.58-win-x64.exe`](releases/v0.8.0.58/1541HUD-V0.8.0.58-win-x64.exe).
Its exact SHA-256 and build provenance are in the adjacent `BUILD-INFO.json`.
The executable was built from the public repository. The owner confirmed that
it launches, its menus are navigable, and ROM/IEC changes return applied/saved
feedback on a loose Fire-24G OneROM. Drive-mounted behavior and media operations
remain untested on this exact packaged executable. The build record's
`hardware_tested` field therefore remains false for full drive qualification.
`tools/onerom-v0.6.0-hud1.exe` is the separate OneROM CLI.

## Install and launch

To run the prebuilt application, download the EXE above and launch it on
Windows x64; Python is not required. Windows may show a SmartScreen or
antivirus warning because the EXE is unsigned. Verify its SHA-256 against
`BUILD-INFO.json` before running it. For source use or to rebuild, install
Python 3.12 with pip, Tcl/Tk, and the Python launcher. From the repository root:

```powershell
py -3 -m pip install -r ./1541hud/requirements.txt
py -3 -c "import tkinter, serial; print('1541HUD dependencies ready')"
./1541hud/source/Run-1541HUD-V0.8.0.58.cmd
```

Tkinter is part of the Python installation; do not install an unrelated
package named tkinter from pip. If `py` is unavailable, repair the Python
launcher installation or use the same Python executable for both the pip
command and the application:

```powershell
python -m pip install -r ./1541hud/requirements.txt
python ./1541hud/source/1541HUD_V0.8.0.58.py
```

Do not install pyserial into a different Python installation from the one
launching HUD. The application can open without pyserial for layout preview,
but USB discovery and communication require it.

## Connect boards

Install the firmware and wiring appropriate to each role using the
[verified wiring guide](../docs/FIRE24G-VERIFIED-WIRING.md) and
[release instructions](../docs/PUBLIC_FIRE24G_RELEASES.md). Assign Control
and optional Monitor roles in Settings using their USB serial identities.
COM port numbers may change after reconnecting. Close other serial terminals
and CLI sessions before using HUD, and close HUD before flashing.

If no boards appear, first verify the dependency import above and normal USB
enumeration. BOOTSEL is for programming, not HUD operation. A loose OneROM
can exercise USB commands but cannot provide live drive telemetry without
the drive and capture wiring. See the [qualification boundary](../docs/VALIDATION_MATRIX.md).

## Tests and optional standalone build

```powershell
py -3 -m unittest discover -s ./1541hud/tests -v
```

The build script uses the pinned dependencies in
`1541hud/requirements-build.txt` and Nuitka's supported Windows compiler
toolchain. Create a Python 3.12 environment, install both requirements files,
then run:

```powershell
py -3.12 -m pip install -r ./1541hud/requirements.txt -r ./1541hud/requirements-build.txt
./1541hud/Build-1541HUD-V0.8.0.58.ps1
```

The script writes the EXE and `BUILD-INFO.json` under
`1541hud/releases/v0.8.0.58/`. The bundled Python, Tcl/Tk, and pyserial
notices are included in `THIRD-PARTY-NOTICES.txt`.
