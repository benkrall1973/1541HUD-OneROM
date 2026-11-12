# 1541HUD Windows application

V0.8.0.51 is the last hardware-qualified application baseline. The new
`source/1541HUD_V0.8.0.53.py` candidate applies IEC addresses through the
firmware's volatile `ROMIEC_NOW` command, so Apply Only changes SEL_A without
writing NV2. The temporary address remains while UB3 stays powered. If cycling
the 1541 must also restart UB3 and reload NV2 while USB data remains connected,
use a data-capable USB cable with host VBUS +5 V blocked, or fully power-cycle
UB3. Use the stock two-ROM image with Control USB plugin v0.8.1.17 for V53;
the older v0.8.1.16 stock image is for the V51 baseline. If a 1541-only power
cycle must restore the saved address while USB data remains connected, UB3
must also lose power: use a data-capable cable with host VBUS +5 V blocked, or
power-cycle UB3 explicitly. V0.8.0.52 is superseded. The HUD version is independent
of OneROM firmware v0.8.1. The
public package includes Python source and launchers, not a prebuilt standalone
HUD EXE. `tools/onerom-v0.6.0-hud1.exe` is the separate OneROM CLI.

## Install and launch

Install Python 3.12 or newer for Windows with pip, Tcl/Tk and the Python launcher. The
automated HUD regression job uses Python 3.12. From the repository root:

```powershell
py -3 -m pip install -r ./1541hud/requirements.txt
py -3 -c "import tkinter, serial; print('1541HUD dependencies ready')"
./1541hud/source/Run-1541HUD-V0.8.0.53.cmd
```

Tkinter is part of the Python installation; do not install an unrelated
package named tkinter from pip. If `py` is unavailable, repair the Python
launcher installation or use the same Python executable for both the pip
command and the application:

```powershell
python -m pip install -r ./1541hud/requirements.txt
python ./1541hud/source/1541HUD_V0.8.0.53.py
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

The optional `Build-1541HUD-V0.8.0.53.ps1` script packages a standalone EXE
with Nuitka. That developer workflow additionally requires Nuitka and its
supported Windows C/C++ compiler toolchain; neither is required to run the
Python source. Install the runtime requirements first. This documentation
update does not publish a newly qualified standalone HUD executable.
