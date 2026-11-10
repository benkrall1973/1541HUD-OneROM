"""Generate local synthetic ZIP fixtures; never touches a device or private ROM."""
import io
import pathlib
import struct
import sys
import zipfile

core = pathlib.Path(sys.argv[1]).resolve()
root = core / 'rust/fw/tests/hud-generated-fixtures'
root.mkdir(parents=True, exist_ok=True)
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
print(root)
