#!/usr/bin/env bash
# Apply only to a disposable export of the pinned official source.
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
core="${1:?Usage: Apply-Host-Dependencies.sh <exported-core-directory>}"
overlay="$root/build-overlays/host-dependencies-v0.8.1.patch"
for file in Makefile rust/fw/Cargo.toml rust/cli/Cargo.toml rust/Cargo.lock; do
  sed -i 's/\r$//' "$core/$file"
done
patch --batch --forward --fuzz=0 -d "$core" -p1 < "$overlay"
python3 - "$root/build-overlays/host-dependencies-v0.8.1.json" "$core/rust/Cargo.lock" "$overlay" <<'PY'
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
print('Host dependency overlay verified: ZIP 9.5.0 / Rustls 0.23.46')
PY
