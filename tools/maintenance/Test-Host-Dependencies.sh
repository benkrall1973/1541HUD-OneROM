#!/usr/bin/env bash
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
core="${1:?Usage: Test-Host-Dependencies.sh <prepared-core-directory>}"
testdir="$core/rust/fw/tests"
export HUD_ZIP_FIXTURES="$testdir/hud-generated-fixtures"
mkdir -p "$HUD_ZIP_FIXTURES"
cp "$root/tools/maintenance/tests/host_dependency_zip.rs" "$testdir/hud_host_dependency_zip.rs"
python3 - "$HUD_ZIP_FIXTURES" <<'PY'
import io, pathlib, struct, sys, zipfile
root = pathlib.Path(sys.argv[1])
payload = bytes(i % 251 for i in range(8192))
for name, method in [('stored.zip', zipfile.ZIP_STORED), ('deflated.zip', zipfile.ZIP_DEFLATED)]:
    out = io.BytesIO()
    with zipfile.ZipFile(out, 'w', compression=method) as archive:
        archive.writestr('roms/synthetic.bin', payload)
        archive.writestr('other.bin', b'not the selected member')
    (root / name).write_bytes(out.getvalue())
stored = (root / 'stored.zip').read_bytes()
(root / 'truncated.zip').write_bytes(stored[:32])
bad = bytearray(stored)
filename, extra = struct.unpack_from('<HH', bad, 26)
bad[30 + filename + extra] ^= 1
(root / 'bad-crc.zip').write_bytes(bad)
(root / 'not-a-zip.bin').write_bytes(b'not a ZIP archive')
PY
cd "$core/rust"
cargo metadata --locked --no-deps --format-version 1 >/dev/null
cargo test --locked -p onerom-fw
cargo test --locked -p onerom-cli --bins
cargo test --locked -p onerom-cli --test image_convert --test image_transform --test general
if [[ -d /sys/bus/usb/devices ]]; then
  cargo test --locked -p onerom-cli --test args --test scan
else
  echo "SKIP: USB-discovery integration suites require /sys/bus/usb/devices (unavailable here)."
fi
echo "PASS: host CLI and ZIP extraction regressions"
