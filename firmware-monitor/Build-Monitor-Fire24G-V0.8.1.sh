#!/usr/bin/env bash
# Build the Fire-24G passive UB4 Monitor image on Linux/WSL2.
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
core="$root/source/one-rom-v0.8.1"
usb="$root/firmware-monitor/active-plugins/system/1541HUD-Monitor-USB"
monitor="$root/firmware-monitor/active-plugins/user/1541HUD-Monitor"
output="${OUTPUT_PATH:-$root/firmware-monitor/releases/v8.1/candidates/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.28.bin}"

: "${TOOLCHAIN:?Set TOOLCHAIN to the directory containing arm-none-eabi-gcc}"
: "${ONEROM_CLI:=onerom}"
export PATH="$TOOLCHAIN:$PATH"

for item in "$TOOLCHAIN/arm-none-eabi-gcc" "$core/Makefile" "$usb/Makefile" "$monitor/Makefile" "$root/firmware-monitor/COMPOSE_UB4_MONITOR_FIRE24G_V081.json"; do
  [[ -e "$item" ]] || { echo "Missing required build input: $item" >&2; exit 1; }
done
command -v make >/dev/null || { echo 'GNU Make is required.' >&2; exit 1; }
command -v cargo >/dev/null || { echo 'Rust/Cargo is required.' >&2; exit 1; }
command -v "$ONEROM_CLI" >/dev/null || { echo "OneROM CLI not found: $ONEROM_CLI" >&2; exit 1; }

make -C "$core" generated
make -C "$core/firmware" -j2 \
  BUILD_DIR=build-ub4-fire24g-v081 \
  BIN_PREFIX=onerom-rp235x-passive-ub4-fire24g-v081 \
  VERSION_MAJOR=0 VERSION_MINOR=8 VERSION_PATCH=1 BUILD_NUMBER=1 GIT_COMMIT=public \
  EXTRA_C_FLAGS=-DHUD1541_PASSIVE_UB4
make -B -C "$usb" -j2 TOOLCHAIN="$TOOLCHAIN" BUILD_DIR=build RWX_SEGMENT_FLAG=-Wl,--no-warn-rwx-segments
make -B -C "$monitor" -j2 TOOLCHAIN="$TOOLCHAIN" BUILD_DIR=build RWX_SEGMENT_FLAG=-Wl,--no-warn-rwx-segments

mkdir -p "$(dirname "$output")"
cd "$root/firmware-monitor"
"$ONEROM_CLI" firmware build \
  --config COMPOSE_UB4_MONITOR_FIRE24G_V081.json \
  --board fire-24-g \
  --base-firmware "$core/firmware/build-ub4-fire24g-v081/onerom-rp235x-passive-ub4-fire24g-v081.bin" \
  --output "$output" --force
sha256sum "$output"
