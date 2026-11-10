# Patched Windows OneROM CLI

Use tools/onerom-v0.6.0-hud1.exe from this repository or its matching release.
It is the pinned upstream OneROM CLI 0.6.0 with the checked host dependency
overlay. The upstream version string is unchanged; hud1 in the filename and
tools/windows-cli-manifest.json identify the community distribution.

## Inspect and discover

Offline firmware inspection does not access a board:

```powershell
./tools/onerom-v0.6.0-hud1.exe --version
./tools/onerom-v0.6.0-hud1.exe firmware inspect --firmware ./releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.32.bin
```

Close HUD before device access. Connect only the intended board, then:

```powershell
./tools/onerom-v0.6.0-hud1.exe --unrecognised scan --board fire-24-g
```

The board option belongs to the scan subcommand in this CLI version.
Record and verify the target serial before any programming. Follow
PUBLIC_FIRE24G_RELEASES.md for the role-specific flashing procedure. Replacing
this executable does not update a board and does not require flashing.

## Build on Windows

Install Git for Windows (including Bash, patch and tar), Python 3, Rust/Cargo
1.99.0 with the x86_64-pc-windows-msvc target, and Visual Studio C++ Build Tools
with a Windows SDK. From the repository root:

```powershell
./tools/maintenance/Build-Windows-CLI.ps1
```

The script exports supported source to .buildtools/windows-cli-core, verifies
its checked-in host fixes and source provenance, runs release-profile ZIP/CLI regressions,
builds the executable and inspects both published images offline. It creates
the executable, hash/provenance manifest, dependency inventory and license
notices in .buildtools/windows-cli-package. It never programs or reboots a
device. CI runs this same script on Windows.

The executable is unsigned. Native build and offline tests do not qualify
USB driver behavior on every Windows installation. Loose-board USB, flashing,
reboot and saved IEC/LED checks passed on the maintainer's Windows host on
2026-11-10; see STOCK_CONTROL_LOOSE_TEST_2026-11-10.md. Full drive behavior
still needs a 1541, and a complete USB power-cycle test remains pending.
