# Private build-input templates

These templates are exported into the new ROM-free repository. Copy the JSON
files into its ignored `private-roms/` directory, provide only ROM images you
are licensed to use, and adjust labels/slots locally. Do not commit either the
filled templates or their referenced files.

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

The project deliberately does not name, hash, fetch, package, or distribute
third-party DOS ROMs in the public repository.
