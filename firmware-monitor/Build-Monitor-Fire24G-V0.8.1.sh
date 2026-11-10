#!/usr/bin/env bash
# Build UB4 Monitor from the tested source; never use the UB3 reset overlay.
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
upstream="$root/source/one-rom-v0.8.1"
core="$root/firmware-monitor/.work/official-v0.8.1/one-rom-v0.8.1"
usb="$root/firmware-monitor/active-plugins/system/1541HUD-Monitor-USB"
monitor="$root/firmware-monitor/active-plugins/user/1541HUD-Monitor"
output="${OUTPUT_PATH:-$root/firmware-monitor/releases/v8.1/candidates/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.30.bin}"
: "${TOOLCHAIN:?Set TOOLCHAIN to the Linux Arm GNU 15.3.Rel1 bin directory}"
: "${ONEROM_CLI:=onerom}"
export PATH="$TOOLCHAIN:$PATH"
mkdir -p "$core" "$(dirname "$output")"
# Export only tracked official source, excluding downloaded .git caches.
git -C "$root" archive HEAD:source/one-rom-v0.8.1 | tar -x -C "$core"
bash "$root/tools/maintenance/Apply-Host-Dependencies.sh" "$core"
sed -i 's/\r$//' "$core/firmware/src/piodma/pio.c" "$core/firmware/src/plugin.c"
patch --fuzz=0 -d "$core" -p1 < "$root/firmware-monitor/overlays/ub4-passive-v0.8.1.patch"
make -C "$core" TOOLCHAIN="$TOOLCHAIN" EXTRA_C_FLAGS=-DHUD1541_PASSIVE_UB4 all
make -C "$core/plugins/system/usb" TOOLCHAIN="$TOOLCHAIN" all
make -B -C "$usb" TOOLCHAIN="$TOOLCHAIN" ONEROM_ROOT="$core" all
make -B -C "$monitor" TOOLCHAIN="$TOOLCHAIN" ONEROM_ROOT="$core" all
cd "$root/firmware-monitor"
"$ONEROM_CLI" firmware build --config COMPOSE_UB4_MONITOR_FIRE24G_V081.json --board fire-24-g --base-firmware "$core/firmware/build/onerom-rp235x.bin" --output "$output" --force
"$ONEROM_CLI" firmware inspect --firmware "$output"
sha256sum "$output"
