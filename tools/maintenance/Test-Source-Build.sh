#!/usr/bin/env bash
# Builds both roles using a synthetic Control ROM. Never programs a device.
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
: "${TOOLCHAIN:?Set TOOLCHAIN to the Arm GNU 15.3.Rel1 bin directory}"
export PATH="$TOOLCHAIN:$PATH"
check="$root/.buildtools/source-check"
mkdir -p "$check"
python3 - "$root" "$check" <<'PY'
import json, pathlib, sys
root, check = map(pathlib.Path, sys.argv[1:])
page = check / 'SYNTHETIC-DO-NOT-FLASH.bin'
page.write_bytes(bytes([255]) * 8192)
sets = []
for path, kind in [
    ('firmware-control/active-plugins/system/1541HUD-USB-v0.8.1/build/1541-OneROM-1541HUD-USB-v0.8.1.16.bin', 'system_plugin'),
    ('firmware-control/active-plugins/user/1541HUD-Host-Control-v0.8.1/build/1541-OneROM-1541HUD-Host-Control-v0.8.1.5.bin', 'user_plugin')]:
    sets.append(dict(type='single', roms=[dict(file=str(root / path), type=kind)]))
sets.append(dict(type='multi', roms=[dict(file=str(page), type='2364', cs1='active_low') for _ in range(3)]))
config = dict(version=1, name='SYNTHETIC BUILD CHECK - DO NOT FLASH',
              description='Build verification only; contains synthetic ROM pages.',
              rom_sets=sets, reserved_pins=['sel_b', 'sel_c', 'sel_d'])
(check / 'compose.json').write_text(json.dumps(config, indent=2))
PY
bash "$root/consolidation/scripts/build-control-v0.8.1.sh" \
  "$check/compose.json" "$check/CONTROL-SYNTHETIC-DO-NOT-FLASH.bin"
export ONEROM_CLI="$root/firmware-control/.work/control-core-v0.8.1/one-rom-v0.8.1/rust/target/release/onerom"
export OUTPUT_PATH="$check/monitor-build-check.bin"
bash "$root/firmware-monitor/Build-Monitor-Fire24G-V0.8.1.sh"
python3 - "$root" "$check" <<'PY'
import hashlib, json, pathlib, subprocess, sys
root, check = map(pathlib.Path, sys.argv[1:])
images = []
for name, scope in [('CONTROL-SYNTHETIC-DO-NOT-FLASH.bin', 'Synthetic ROM pages; never flash'),
                    ('monitor-build-check.bin', 'Build verification only; not the hardware-accepted release image')]:
    path = check / name
    images.append(dict(file=name, bytes=path.stat().st_size, sha256=hashlib.sha256(path.read_bytes()).hexdigest(), scope=scope))
record = dict(source_commit=subprocess.check_output(['git', '-C', str(root), 'rev-parse', 'HEAD'], text=True).strip(),
              cargo=subprocess.check_output(['cargo', '--version'], text=True).strip(),
              compiler=subprocess.check_output(['arm-none-eabi-gcc', '--version'], text=True).splitlines()[0],
              host_overlay_sha256=hashlib.sha256((root / 'overlays/host-dependencies-v0.8.1.patch').read_bytes()).hexdigest(),
              source_hardening_sha256=hashlib.sha256((root / 'source/one-rom-v0.8.1/1541HUD-SOURCE.json').read_bytes()).hexdigest(),
              images=images, hardware_programmed=False)
(check / 'build-verification.json').write_text(json.dumps(record, indent=2) + '\n')
PY
echo "PASS: both firmware roles built and inspected. No device was programmed."
