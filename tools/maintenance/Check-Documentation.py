"""Validate tracked project Markdown file targets without editing vendored docs."""
from pathlib import Path
from urllib.parse import unquote, urlsplit
import re
import subprocess
import sys

root = Path(__file__).resolve().parents[2]
result = subprocess.run(['git', 'ls-files', '-z', '--', '*.md'], cwd=root,
                        check=True, capture_output=True)
files = [s for s in result.stdout.decode('utf-8').split('\0') if s]
errors = []
count = 0
for name in files:
    if name.startswith(('source/', 'firmware-control/source/')):
        continue
    path = root / name
    text = path.read_text(encoding='utf-8-sig')
    text = re.sub(r'```.*?```', '', text, flags=re.S)
    for match in re.finditer(r'\]\((<[^>]+>|[^\s)]+)(?:\s+"[^"]*")?\)', text):
        target = match[1].strip('<>')
        parsed = urlsplit(target)
        if parsed.scheme or parsed.netloc or not parsed.path:
            continue
        count += 1
        base = root if parsed.path.startswith('/') else path.parent
        destination = base / unquote(parsed.path.lstrip('/'))
        if not destination.exists():
            line = text.count('\n', 0, match.start()) + 1
            errors.append(f'{name}:{line}: missing target {target}')
if errors:
    print('\n'.join(errors), file=sys.stderr)
    sys.exit(1)
print(f'PASS: {count} project Markdown file targets resolve; upstream document links excluded.')
