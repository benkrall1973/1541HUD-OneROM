"""Verify the checked-in supported host source and its dependency provenance."""
from pathlib import Path
import hashlib,json,re,sys
root=Path(__file__).resolve().parents[2]
core=Path(sys.argv[1]) if len(sys.argv)>1 else root/'source/one-rom-v0.8.1'
name='1541HUD-SOURCE.json'
expected=json.loads((root/'source/one-rom-v0.8.1'/name).read_text())
actual=json.loads((core/name).read_text())
if actual!=expected:raise SystemExit('Source provenance differs from repository manifest')
for relative,digest in expected['files_sha256_lf'].items():
    data=(core/relative).read_bytes().replace(b'\r\n',b'\n')
    if hashlib.sha256(data).hexdigest()!=digest:
        raise SystemExit('Supported source checksum mismatch: '+relative)
for relative in expected['excluded_unsupported_trees']:
    if (core/relative).exists():raise SystemExit('Unsupported build tree present: '+relative+'; remove the stale exported build directory and rebuild from current source')
lock=(core/'rust/Cargo.lock').read_text()
for package,version in expected['packages'].items():
    found=re.findall(r'(?m)^name = "'+re.escape(package)+r'"\nversion = "([^\"]+)"$',lock)
    if found!=[version]:raise SystemExit(f'Unexpected {package} resolution: {found}')
if re.search(r'(?m)^name = "lru"$',lock):raise SystemExit('Excluded Studio LRU dependency remains')
print('PASS: checked-in supported source, pinned dependencies, and excluded GUI trees verified.')
