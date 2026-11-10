#!/usr/bin/env bash
# Verify current supported source; retain legacy overlay support for old exports.
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
core="${1:?Usage: Apply-Host-Dependencies.sh <exported-core-directory>}"

if [[ -f "$core/1541HUD-SOURCE.json" ]]; then
  python3 "$root/tools/maintenance/Verify-Source-Dependencies.py" "$core"
  exit 0
fi
overlay="$root/overlays/host-dependencies-v0.8.1.patch"
for file in Makefile rust/fw/Cargo.toml rust/cli/Cargo.toml rust/studio/Cargo.toml rust/Cargo.lock; do
  sed -i 's/\r$//' "$core/$file"
done
patch --batch --forward --fuzz=0 -d "$core" -p1 < "$overlay"
python3 - "$root/overlays/host-dependencies-v0.8.1.json" "$core/rust/Cargo.lock" "$overlay" <<'PY'
import hashlib, json, pathlib, re, sys
manifest, lock, patch = map(pathlib.Path, sys.argv[1:])
expected = json.loads(manifest.read_text())
for path, key in [(lock, 'lock_sha256'), (patch, 'patch_sha256')]:
    if hashlib.sha256(path.read_bytes()).hexdigest() != expected[key]:
        raise SystemExit('Host dependency checksum mismatch: ' + path.name)
for name, version in expected['packages'].items():
    found = re.findall(r'(?m)^name = "' + re.escape(name) + r'"\nversion = "([^\"]+)"$', lock.read_text())
    if found != [version]:
        raise SystemExit(f'Unexpected {name} resolution: {found}')
print('Host dependency overlay verified: ' + ', '.join(f'{k} {v}' for k, v in expected['packages'].items()))
PY
