# Build the UB3 Control firmware

This guide builds a flashable UB3 Control image from a fresh checkout. If you
only want to use the supplied image, skip this and follow
[Flash a release image](PUBLIC_FIRE24G_RELEASES.md).

## What you need

- A Windows PC with **WSL2 and Ubuntu**, or a Linux computer.
- Internet access. The build downloads pinned source dependencies; the stock
  DOS page is included in the checksummed ROM library.
- The build tools below. The supported, CI-matched versions are Rust/Cargo
  **1.99.0** and Arm GNU Toolchain **15.3.rel1**.
- A ROM configuration and the ROM pages it names. The public stock
  configuration is ready to use. For copyrighted ROMs, provide your own
  licensed files and keep them and your finished configuration out of Git.

The build runs in WSL/Linux because the OneROM source uses Linux shell commands.
The OneROM CLI is the final image composer in this workflow. By itself it does
not compile the project plugins or the patched OneROM core.

## 1. Install the build tools

In Ubuntu/WSL, install the basic packages:

```bash
sudo apt update
sudo apt install -y build-essential git make patch tar python3 curl xz-utils
```

Install `rustup` if needed, then install and select Rust 1.99.0:

```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
source "$HOME/.cargo/env"
rustup toolchain install 1.99.0 --profile minimal
rustup default 1.99.0
```

From the repository root, install the pinned Arm compiler and add it to this
terminal's search path:

```bash
export TOOLCHAIN="$(bash source/one-rom-v0.8.1/ci/install-arm-toolchain.sh "$HOME/arm-gnu-toolchain" 15.3.rel1)"
export PATH="$TOOLCHAIN:$PATH"
```

Run these commands again in a new terminal before building, or add them to your
shell startup file.

## 2. Choose a configuration

For a source-built stock two-ROM image, use the checked-in configuration:

```text
releases/stock-2rom-compose.json
```

The repository currently builds USB plugin **0.8.1.18**. The attached stock
release image uses USB **0.8.1.16** and remains the hardware-tested artifact;
a fresh source build is a separate candidate until it is flashed and checked.
The 901229-06AA ROM page is included in the checksummed library, so the stock
composition has no external ROM download.

The build wrapper applies two small patches to the disposable upstream copy:
the UB3 reset interlock and an initialization for OTP output variables that
GCC 15 otherwise reports as possibly uninitialized. The OTP helper returns
before those values are read when no commissioning data exists, so this change
does not alter normal behavior.

The reset interlock is the important UB3-specific part: it asserts reserved
SEL_B early in core startup so the 1541 remains held in reset while ROM serving
restarts. Plugins start after ROM serving has been set up, so moving this
assertion into the current USB plugin would be too late to provide the same
startup guarantee. Keep the core overlay in this build unless the reset
sequence is redesigned and verified or an upstream firmware hook replaces it.

## Can I use OneROM Studio?

Studio can load a OneROM JSON configuration and build a ROM collection using
prebuilt ROM and plugin files. It uses an official published OneROM base
firmware selected in Studio. It does not compile this repository's C plugins
or apply the UB3 early-reset core overlay, so a Studio-only build is not the
same as the tested UB3 Control image.

For the complete UB3 image, use this guide's build script. It builds the core
and plugins, then invokes the OneROM CLI with the resulting patched base
firmware and the selected configuration. If you have already built those
inputs, the CLI can compose the exact image directly; see the advanced command
in [the composition guide](UB3_CONTROL_CLI_COMPOSITION_GUIDE.md). The current
templates use the v0.8.1 `chip_sets` schema and are suitable inputs to that CLI
workflow.

For your own ROM set, copy the example matching your wiring into the ignored
`roms/private/` folder and update its file paths. The quick rule is:

- **Two ROM pages, no 74LS138:** choose Lower Socket or Upper Socket.
- **Three C/A/E pages and the documented 74LS138 wiring:** choose the C/A/E
  template. Each page must be exactly 8 KiB; keep the C, A, E order.

See the [ROM layout and manifest reference](UB3_CONTROL_CLI_COMPOSITION_GUIDE.md)
for the examples, file names, and full C/A/E details. The
[ROM catalog](../roms/README.md) lists included public files and required
C64 KERNAL pairings.

For example, to start a custom three-page setup:

```bash
mkdir -p roms/private
cp roms/configs/control-compose.template.json roms/private/my-drive.json
```

Edit `roms/private/my-drive.json` to point to your local 8 KiB C, A, and E
pages. For the two-page templates, copy the matching
`roms/configs/UB3-Control-Lower-Socket.template.json` or
`roms/configs/UB3-Control-Upper-Socket.template.json` instead.

## 3. Build

From the repository root in WSL/Linux, run:

```bash
bash consolidation/scripts/build-control-v0.8.1.sh \
  releases/stock-2rom-compose.json \
  output/onerom-ub3-control-fire24g.bin
```

For a private configuration, replace the first path with your JSON file, for
example `roms/private/my-drive.json`. The script builds the Control core,
plugins, and local OneROM command-line tool, then composes and inspects the
image. The finished file and its SHA-256 are reported at the end.

To compose the four-profile no-JiffyDOS layout from the public, checksummed
ROM pages, use:

```bash
bash consolidation/scripts/build-control-v0.8.1.sh \
  releases/r21-basic-no-jiffydos-compose.json \
  output/onerom-ub3-basic-no-jiffydos.bin
```

This produces an **untested build candidate**. It uses the C/A/E page layout
and requires the documented 74LS138 wiring. Its fresh Host-Control NV starts
erased; USB plugin 0.8.1.18 seeds the saved ROM slot to 1 and IEC address to 8
when it initializes. The published r21 binary is a separate, offline-inspected
release artifact and still needs the same drive test.

The pinned upstream OneROM core is exported from the current Git commit.
Commit changes to that core before building; uncommitted core edits are not
included. The project Control plugin source is built from the checkout.
ROM configurations and ROM pages can remain local and uncommitted.

## 4. Before flashing

Check that the output path names the image you intended. Flash only after
reading the [wiring guide](FIRE24G-VERIFIED-WIRING.md) and the
[release and recovery instructions](PUBLIC_FIRE24G_RELEASES.md). Close
1541HUD, scan for the board, confirm its serial number, then program with
verification enabled. Do not guess which connected board is UB3.

For build internals and CI evidence, see
[clean source-build notes](CLEAN_SOURCE_BUILD.md) and the
[v8.1 build/install record](FIRE24G_V81_BUILD_AND_INSTALL.md).
