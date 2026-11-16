# ROM composition templates

These JSON files describe OneROM layouts. Copy a suitable template into
`private-roms/`, update its labels and file paths, then build an image with the
documented tool. Keep licensed ROM files and completed private configurations
out of Git.

For the public CLI workflow and the qualified three-page Fire-24G Control
layout, read
`docs/UB3_CONTROL_CLI_COMPOSITION_GUIDE.md`.

- `UB3-Control-Lower-Socket.template.json` creates a standard two-page C/E
  Control image; it does not require a RAMBoard.
- `UB3-Control-Upper-Socket.template.json` creates the corresponding E/C
  Control image; it does not require a RAMBoard.
- `control-compose.template.json` creates a three-page C/A/E Control image for
  the documented Fire-24G decode wiring, including DolphinDOS-style ROM sets.
- `monitor-compose.template.json` creates the UB4 passive Monitor image after
  the owner supplies the two legally held 8 KiB monitor/boot ROM inputs.

The public repository includes a small, hash-listed ROM library. For ROMs not
included there, provide only files you are entitled to use. See the
[ROM catalog](../docs/rom-library/README.md) for what is included and the
[package boundary](../docs/PUBLIC_PACKAGE_MANIFEST.md) for what may be added.
