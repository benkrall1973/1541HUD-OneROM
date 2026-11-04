# 1541HUD Fire-24G V8.1 Build and Installation Record

This is the active, hardware-verified configuration for the paired OneROM
installation in the 1541 long board. UB3 is the state-changing **Control**
board; UB4 is the electrically passive **Monitor** board. Do not exchange
their firmware, USB roles, or wiring.

## Confirmed board identity

| Board | Role | Active firmware |
| --- | --- | --- |
| UB3 | Control | OneROM V8.1 Control, 8-ROM 74LS138 image |
| UB4 | Monitor | 1541HUD Monitor `v0.8.1.28` |

Windows COM numbers are not identities and may change after every reconnect.
Bind 1541HUD by each board's USB serial as discovered locally; Windows COM
numbers are not identities and must not be documented as a deployment contract.

## Fire-24G GPIO assignment

| Board | OneROM signal / GPIO | 1541 endpoint | Purpose |
| --- | --- | --- | --- |
| UB3 | SEL_A / GPIO26 | existing J1 address-control wire | IEC device 8/9 control |
| UB3 | SEL_B / GPIO27 | C46 positive / CR7 anode through **1 kOhm** | active-low 1541 reset |
| UB3 | SEL_C / GPIO25 | 5.1 kOhm to KSC2383YTA base | temporary write-protect override |
| UB3 | SEL_D / GPIO24 | P6 pin 12 through 22 kOhm; 22 kOhm from SEL_D to GND | physical write-protect sense |
| UB3 | X1 / GPIO9 | 74LS138 pin 10, `/Y5` | `$A000-$BFFF` page select |
| UB3 | X2 / GPIO8 | 74LS138 pin 7, `/Y7` | `$E000-$FFFF` page select |
| UB4 | SEL_A / GPIO26 | UC2 pin 23, `/CS2` | passive VIA2 select capture |
| UB4 | SEL_B / GPIO27 | UC2 pin 17, PB7 / active-low SYNC | raw SYNC/RPM capture |
| UB4 | X1 / GPIO9 | UC4 pin 34, `R/W` | passive write-cycle qualification |
| UB4 | X2 / GPIO8 | UC4 pin 39, `phi2` | passive bus timing capture |

UB4 has **no write-protect wiring or write-protect authority**. Write-protect
override and physical status are UB3-only features.

### Reset and write-protect rules

- SEL_B is only ever driven LOW then released; it is never driven HIGH.
- The fitted 1 kOhm reset resistor is required. The former 2.2 kOhm value did
  not discharge C46 adequately and could leave IEC reads stalled after reset.
- `ROMRESET` holds SEL_B LOW for **350 ms**, then releases it. OneROM boot and
  reboot also hold the node LOW for 350 ms until ROM service is available.
- SEL_C reaches P6 pin 12 only through the 5.1 kOhm base resistor and NPN.
  Never connect a OneROM output directly to P6 pin 12 or UC2.
- SEL_D is 3.3 V input-only. The 22 kOhm / 22 kOhm divider is mandatory.

## Build entry points

All project source, composition manifests and final tested images are checked
into this branch. Generated object directories and third-party tool executables
are intentionally not committed; install the pinned tool prerequisites below.

| Component | Build command | Expected output |
| --- | --- | --- |
| UB3 Control, WSL2/Linux source build | `bash consolidation/scripts/build-control-v0.8.1.sh private-roms/<your-config>.json output/onerom-ub3-control-fire24g.bin` | `output/onerom-ub3-control-fire24g.bin` |
| UB4 Monitor, WSL2/Linux source build | `TOOLCHAIN=/path/to/bin ONEROM_CLI=onerom bash firmware-monitor/Build-Monitor-Fire24G-V0.8.1.sh` | `firmware-monitor/releases/v8.1/candidates/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.28.bin` |

The Control wrapper builds the local patched V8.1 base, Control USB plugin and
Host-Control plugin before composing the owner-supplied manifest. The Monitor build must use the passive core define
`HUD1541_PASSIVE_UB4`; do not build its plugins with a normal ROM-serving core.

### Supported build host: WSL2 on Windows

For Windows users, use **WSL2 with Ubuntu** as the supported public build
environment. The upstream firmware Makefiles are POSIX Makefiles: they use
`sh`, `mkdir -p`, `rm`, `find`, pipes and Unix path rules. WSL2 provides those
tools as normal tools rather than hoping a particular Windows Make/shell pairing
happens to emulate them correctly.

Use `firmware-monitor/Build-Monitor-Fire24G-V0.8.1.sh` from WSL2/Linux. It
builds the passive base, both Monitor plugins and the final Fire-24G
composition. The existing `.ps1` remains an engineering script for a prepared
native Windows environment, not the public-supported route.

Native Windows with MSYS2/Git Bash may work for experienced users, but is not
the baseline procedure and must not be represented as tested merely because
`make.exe` exists on PATH. Windows has already contributed enough drama.

### Prerequisites for a fresh checkout

- ARM GNU Toolchain 14.2 or compatible `arm-none-eabi-gcc` on `PATH`.
- GNU Make and a POSIX shell (`sh`) in WSL2/Linux.
- OneROM CLI 0.5.0 or compatible `onerom` executable. The validated Windows
  executable is included at `tools/onerom-v0.5.0.exe`; place it on `PATH` or
  pass its path to the build wrapper where needed.
- Rust/Cargo where the Control source build requests it.

The Monitor PowerShell entry point also accepts `-Toolchain`, `-Make`, and
`-OneRomCli` arguments for an installed-but-not-on-PATH toolchain. This keeps
the repository portable instead of baking in somebody's Desktop path, because
that sort of thing is how "reproducible" builds become performance art.
`Build-Control-V8.1-Clean.cmd` remains an optional native Windows composition
helper for an already-built core and plugins. Invoke it as
`Build-Control-V8.1-Clean.cmd private-roms/<your-config>.json output/<name>.bin`.
It is not the supported fresh public source-build path.

## Flash and acceptance procedure

1. Shut down 1541HUD so it releases both CDC ports.
2. Use `onerom --unrecognised scan` and verify the target serial. Do not flash
   the other board because two red boards look alike, obviously.
3. Program with `--verify`, reboot normally, and inspect the installed plugin
   versions/slots before reconnecting 1541HUD.
4. UB3: verify ROM save/readback, IEC 8 and 9 save/readback, write-protect
   OFF/ON/OFF, then `ROMRESET` followed by an immediate directory read.
5. UB4: verify green idle, a density colour while reading, physical-header and
   raw-SYNC RPM telemetry, and no effect on drive operation.

## Monitor RGB state code

During active motor capture, the LED represents current density: D3 yellow,
D2 blue, D1 orange, D0 white. Solid green is idle/waiting; amber blinking is a
recent capture/event overflow and clears after 1.5 seconds without a new
overflow; red fast blinking is a hardware/resource fault; purple is the short
boot heartbeat. While no HUD CDC client is connected, telemetry events are
discarded rather than accumulating into a false amber warning. Status states
override density.
