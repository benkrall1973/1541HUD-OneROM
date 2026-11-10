#!/usr/bin/env bash
# Fail for any active workspace vulnerability, yanked package or unsoundness.
set -euo pipefail
core="${1:?Usage: Audit-Host-Dependencies.sh <prepared-core-directory> [advisory-db]}"
db="${2:-$core/.host-rustsec-db}"
if [[ ! -d "$db/.git" ]]; then
  git clone --depth 1 https://github.com/RustSec/advisory-db.git "$db"
fi
report="$core/host-dependency-audit.json"
tree="$core/host-dependency-tree.txt"
set +e
cargo audit --json --no-fetch --db "$db" --file "$core/rust/Cargo.lock" > "$report"
result=$?
set -e
if [[ "$result" != 0 && "$result" != 1 ]]; then exit "$result"; fi
cd "$core/rust"
cargo tree --locked --target all --edges normal,build --prefix none --format '{p}' \
  -p onerom-cli -p onerom-metadata > "$tree"
python3 - "$report" "$tree" <<'PY'
import json, pathlib, re, sys
report = json.loads(pathlib.Path(sys.argv[1]).read_text())
if report.get('error') or 'vulnerabilities' not in report:
    raise SystemExit('Advisory scan did not complete')
active = set()
for line in pathlib.Path(sys.argv[2]).read_text().splitlines():
    match = re.match(r'(\S+) v([^\s(]+)', line)
    if match:
        active.add(match.groups())
failed = []
records = report['vulnerabilities']['list']
for record in records:
    package = record['package']
    key = (package['name'], package['version'])
    scope = 'SUPPORTED HOST BUILD' if key in active else 'other workspace dependency'
    print(record['advisory']['id'], *key, scope)
    failed.append(record['advisory']['id'])
print(f'Workspace vulnerabilities: {len(records)}; supported host build: {sum((r["package"]["name"], r["package"]["version"]) in active for r in records)}')
print('Advisory warnings:', ', '.join(report.get('warnings', {})) or 'none')
for kind, warnings in report.get('warnings', {}).items():
    for warning in warnings:
        package = warning['package']
        key = (package['name'], package['version'])
        scope = 'SUPPORTED HOST BUILD' if key in active else 'other workspace dependency'
        advisory = (warning.get('advisory') or {}).get('id', 'no advisory ID')
        print(kind, advisory, *key, scope)
        if kind in ('yanked', 'unsound'):
            failed.append(advisory)
if failed:
    raise SystemExit('Dependency audit failed: ' + ', '.join(failed))
PY
git -C "$db" rev-parse HEAD
