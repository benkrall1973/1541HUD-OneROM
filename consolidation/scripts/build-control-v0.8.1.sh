#!/usr/bin/env bash
# Build a Fire-24G UB3 Control image from tracked source and an owner-supplied
# ROM manifest. The manifest and its ROM pages deliberately stay outside Git.
set -euo pipefail

if [[ $# -lt 1 || $# -gt 2 ]]; then
  echo "Usage: $0 <private-config.json> [output.bin]" >&2
  exit 64
fi

root="$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)"
config="$(CDPATH= cd -- "$(dirname -- "$1")" && pwd)/$(basename -- "$1")"
output="${2:-$root/output/onerom-ub3-control-fire24g.bin}"
upstream="$root/source/one-rom-v0.8.1"
work="$root/firmware-control/.work/control-core-v0.8.1"
core="$work/one-rom-v0.8.1"
usb="$root/firmware-control/active-plugins/system/1541HUD-USB-v0.8.1"
host="$root/firmware-control/active-plugins/user/1541HUD-Host-Control-v0.8.1"

[[ -f "$config" ]] || { echo "Private config not found: $config" >&2; exit 66; }
command -v arm-none-eabi-gcc >/dev/null
command -v cargo >/dev/null
command -v make >/dev/null
command -v git >/dev/null
[[ -f "$upstream/Makefile" ]] || { echo "Pinned upstream source not found: $upstream" >&2; exit 1; }

cargo_version="$(cargo --version | awk '{print $2}')"
case "$(printf '%s\n%s\n' '1.93.0' "$cargo_version" | sort -V | head -n1)" in
  1.85.0) ;;
  *) echo "Cargo $cargo_version is too old; need Cargo 1.93 or newer." >&2; exit 1 ;;
esac

mkdir -p "$(dirname -- "$output")"
# Build from the exact official One ROM v0.8.1 source tracked in this tree.
# UB3 behaviour lives in the paired custom plugins, not in a forked core.
rm -rf "$work"
mkdir -p "$work"
cp -a "$upstream" "$core"
make -C "$core" TOOLCHAIN="${TOOLCHAIN:-/usr/bin}" all
(cd "$core/rust" && cargo build --release -p onerom-cli)
make -C "$usb" clean
make -C "$usb" ONEROM_ROOT="$core"
make -C "$host" clean
make -C "$host" ONEROM_ROOT="$core"

"$core/rust/target/release/onerom" --verbose firmware build \
  --board fire-24-g \
  --config "$config" \
  --base-firmware "$core/firmware/build/onerom-rp235x.bin" \
  --output "$output" \
  --force
"$core/rust/target/release/onerom" firmware inspect --firmware "$output"
sha256sum "$output"
