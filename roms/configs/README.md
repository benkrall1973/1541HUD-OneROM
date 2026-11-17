# ROM composition examples

These four JSON files use the OneROM v0.8.1 `chip_sets` configuration format.
They are ROM-free templates; they do not include private or licensed payloads.
The CLI and Studio can load schema-compatible JSON configurations. Studio
builds against an official published OneROM base firmware; it does not apply
the UB3-specific early-reset core patch described in the build guide.

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
place for private configuration files and owner-supplied ROM data. The custom
plugin `.bin` files referenced by Control and Monitor examples are generated
by their project build scripts; they are not committed to this repository.

Studio can build a configuration from prebuilt ROM and plugin files, but the
result uses the Studio-selected official base firmware. That is useful for a
generic OneROM image, but it is not the complete UB3 Control build while the
early-reset core patch is required. Use the project build script below for the
tested UB3 firmware.

For a custom Control build, invoke the build script from the repository root:

```sh
bash consolidation/scripts/build-control-v0.8.1.sh \
  roms/private/my-drive.json output/my-drive.bin
```

For a stock image, use the checked-in release composition in `releases/`.
Do not copy licensed ROMs or generated custom images into the public tree.
See the [ROM and KERNAL catalog](../README.md) and the
[UB3 build guide](../../docs/BUILD_UB3_CONTROL.md).
