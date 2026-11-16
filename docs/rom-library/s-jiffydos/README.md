# S-JiffyDOS 1541

## What it is

S-JiffyDOS 1541 (SJD41) is a **drive-side patch for JiffyDOS 1541**. It is
not DolphinDOS, and it is independent of the separate S-JiffyDOS 64 project.
The private project generated its S-JiffyDOS 1541 image from a licensed
JiffyDOS 1541-II 6.00 input using the original S-JiffyDOS generator. The
resulting build is split into the `$8000-$9FFF`, `$A000-$BFFF`, `$C000-$DFFF`,
and `$E000-$FFFF` 8 KiB regions for this OneROM/RAMBoard arrangement.

## Inputs and host KERNAL

- A lawful, matching JiffyDOS 1541 source image.
- A lawful copy of the S-JiffyDOS 1541 generator/tool and its instructions.
- OneROM C/A/E wiring plus a working RAMBoard exposing RAM at `$8000-$9FFF`.
- For JiffyDOS serial fast transfers, use a JiffyDOS C64 KERNAL. The original
  S-JiffyDOS 1541 documentation treats SJD41 and SJD64 as independent; a
  special S-JiffyDOS C64 KERNAL is not a prerequisite for the drive patch.

The official JiffyDOS source is commercial; start at the [official JiffyDOS
site](https://www.jiffydos.com/). This public repo does not include the
generator disk, licensed source ROM, or its manual.

## Build and OneROM packaging

The original generator reads the JiffyDOS drive ROM and creates intermediate
files before producing the final S-JiffyDOS image. Follow the instructions
that accompany a lawful copy of that generator; do not substitute a random
16 KiB 1541-II dump or split an unknown image by guesswork. Record the source
model/version and verify the generated output size before splitting it into
four 8192-byte pages.

The OneROM integration is more than a normal four-page ROM selection. On
startup, the tested bootstrap copies the `$A000-$BFFF` shadow-source page into
RAMBoard SRAM at `$8000-$9FFF`, verifies the copy, then starts the S-JiffyDOS
runtime profile. Use the private release runbook for the tested image's exact
profile/build procedure; the public repository does not distribute those
third-party ROM pages or bootstrap image.

## Critical RAMBoard warning

While S-JiffyDOS is active, the RAMBoard `$8000-$9FFF` region contains the
shadowed S-JiffyDOS code. **Any software that writes into that RAM range can
overwrite the live shadow and corrupt the running DOS.** Do not run RAMBoard
memory tests, RAM expansion software, direct memory writes, or programs that
claim/use that region while S-JiffyDOS is active. A full drive power cycle
clears the volatile RAM; run the bootstrap again before selecting the
S-JiffyDOS runtime.

This limitation is specific to the shadow-RAM S-JiffyDOS build. It is not a
claim that every ordinary RAMBoard installation reserves `$8000-$9FFF` in
software.
