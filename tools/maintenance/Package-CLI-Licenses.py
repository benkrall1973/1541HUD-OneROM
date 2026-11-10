"""Collect license notices for the Windows CLI normal/build dependency graph."""
import json
import pathlib
import subprocess
import sys

core, output = map(pathlib.Path, sys.argv[1:])
metadata = json.loads(subprocess.check_output(
    ['cargo', 'metadata', '--locked', '--format-version', '1', '--filter-platform', 'x86_64-pc-windows-msvc'],
    cwd=core / 'rust', text=True))
packages = {p['id']: p for p in metadata['packages']}
nodes = {n['id']: n for n in metadata['resolve']['nodes']}
pending = [p['id'] for p in packages.values() if p['name'] == 'onerom-cli']
seen = set()
while pending:
    package = pending.pop()
    if package in seen:
        continue
    seen.add(package)
    for dep in nodes[package]['deps']:
        if any(kind['kind'] != 'dev' for kind in dep['dep_kinds']):
            pending.append(dep['pkg'])
inventory, notices = [], []
for package in sorted((packages[p] for p in seen), key=lambda p: (p['name'], p['version'])):
    root = pathlib.Path(package['manifest_path']).parent
    files = {p for p in root.iterdir() if p.is_file() and p.name.upper().startswith(('LICENSE', 'LICENCE', 'COPYING', 'NOTICE', 'COPYRIGHT', 'AUTHORS'))}
    if package.get('license_file'):
        files.add(root / package['license_file'])
    texts = []
    for path in sorted(files):
        if path.is_file():
            texts.append(path.name + '\n' + path.read_text(encoding='utf-8', errors='replace'))
    # OneROM workspace packages inherit the upstream root license.
    if not texts and package['source'] is None:
        texts.append('OneROM upstream LICENSE.md\n' + (core / 'LICENSE.md').read_text())
    if not texts:
        raise SystemExit('Missing dependency license text: ' + package['name'])
    inventory.append(dict(name=package['name'], version=package['version'], license=package['license'], repository=package.get('repository')))
    notices.append('\n' + '=' * 72 + '\n' + package['name'] + ' ' + package['version'] + '\nLicense: ' + str(package['license']) + '\n' + '\n\n'.join(texts))
(output / 'THIRD-PARTY-LICENSES.txt').write_text('OneROM CLI Windows distribution dependency notices\nNormal/build dependencies, including compile-time tools.\n' + '\n'.join(notices), encoding='utf-8')
(output / 'windows-cli-dependencies.json').write_text(json.dumps(inventory, indent=2) + '\n', encoding='utf-8')
print('Collected license notices:', len(inventory), 'packages')
