# ROM composition examples

These four JSON files are the single tracked examples for composing ROMs with
the OneROM CLI. Each is ROM-free; they do not include private or licensed
payloads.

| File | Wiring/layout |
|---|---|
| `UB3-Control-Lower-Socket.template.json` | Standard C/E lower-socket build. |
| `UB3-Control-Upper-Socket.template.json` | Standard E/C upper-socket build. |
| `control-compose.template.json` | C/A/E build with the documented 74LS138 decode wiring. |
| `monitor-compose.template.json` | UB4 Monitor build with owner-supplied ROM pages. |

The CLI resolves file paths from the repository working directory. The
examples therefore use repository-relative plugin paths and point DOS/Monitor
inputs into `roms/private/`. Copy the chosen example there, then edit labels
and page paths as needed. That directory is ignored by Git and is the only
place for private configuration files and owner-supplied ROM data.

For a custom Control build, invoke the build script from the repository root:

```sh
bash consolidation/scripts/build-control-v0.8.1.sh \
  roms/private/my-drive.json output/my-drive.bin
```

For a stock image, use the checked-in release composition in `releases/`.
Do not copy licensed ROMs or generated custom images into the public tree.
See the [ROM and KERNAL catalog](../README.md) and the
[UB3 build guide](../../docs/BUILD_UB3_CONTROL.md).
