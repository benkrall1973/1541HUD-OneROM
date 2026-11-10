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
overlay="$root/firmware-control/overlays/ub3-early-reset-v0.8.1.patch"
usb="$root/firmware-control/active-plugins/system/1541HUD-USB-v0.8.1"
host="$root/firmware-control/active-plugins/user/1541HUD-Host-Control-v0.8.1"

[[ -f "$config" ]] || { echo "Private config not found: $config" >&2; exit 66; }
command -v arm-none-eabi-gcc >/dev/null
command -v cargo >/dev/null
command -v make >/dev/null
command -v git >/dev/null
[[ -f "$upstream/Makefile" ]] || { echo "Pinned upstream source not found: $upstream" >&2; exit 1; }

cargo_version="$(cargo --version | awk '{print $2}')"
if [[ "$(printf '%s\n%s\n' '1.93.0' "$cargo_version" | sort -V | head -n1)" != "1.93.0" ]]; then
  echo "Cargo $cargo_version is too old; need Cargo 1.93 or newer." >&2
  exit 1
fi

mkdir -p "$(dirname -- "$output")"
# Build from the exact official One ROM v0.8.1 source tracked in this tree.
# The narrow metadata-gated overlay is a UB3 safety interlock, not a fork.
rm -rf "$work"
mkdir -p "$work"
mkdir -p "$core"
git -C "$root" archive HEAD:source/one-rom-v0.8.1 | tar -x -C "$core"
# The tracked Windows checkout carries CRLF in this source file; normalise the
# disposable build copy before applying the POSIX patch.
sed -i 's/\r$//' "$core/firmware/src/rp235x.c"
# Fail immediately if the reset overlay cannot be applied.
patch --fuzz=0 -d "$core" -p1 < "$overlay"
grep -Fq "UB3 Control reset interlock" "$core/firmware/src/rp235x.c" || {
  echo "Failed to apply UB3 reset interlock overlay." >&2
  exit 1
}
make -C "$core" TOOLCHAIN="${TOOLCHAIN:-/usr/bin}" all
(cd "$core/rust" && cargo build --release -p onerom-cli)
# The official USB plugin fetches its own explicitly pinned TinyUSB,
# picobootx, and pico-sdkless inputs.  Build it once to populate and verify
# those exact dependencies; the custom USB plugin then uses the same copies.
make -C "$core/plugins/system/usb" TOOLCHAIN="${TOOLCHAIN:-/usr/bin}"
make -C "$usb" clean
make -C "$usb" ONEROM_ROOT="$core" all
make -C "$host" clean
make -C "$host" ONEROM_ROOT="$core" all

"$core/rust/target/release/onerom" --verbose firmware build \
  --board fire-24-g \
  --config "$config" \
  --base-firmware "$core/firmware/build/onerom-rp235x.bin" \
  --output "$output" \
  --force
"$core/rust/target/release/onerom" firmware inspect --firmware "$output"
sha256sum "$output"
