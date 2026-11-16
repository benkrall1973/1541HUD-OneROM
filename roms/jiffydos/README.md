# JiffyDOS drive ROM

## What it is

JiffyDOS is a commercial DOS replacement with a drive-side ROM and a matching
computer-side KERNAL. Its high-speed protocol is serial; do not confuse it
with a parallel fastloader.

## Required lawful inputs

- A licensed JiffyDOS drive ROM for the exact 1541 family being targeted.
- A licensed JiffyDOS C64 KERNAL if you want JiffyDOS host-side fast serial
  transfers. A stock C64 KERNAL can still use ordinary IEC.
- The official install/user documentation for the licensed release.

Get the software and manuals from the [official JiffyDOS
site](https://www.jiffydos.com/). The public repo links there rather than
re-hosting the manuals or ROM bytes.

## Build procedure

1. Keep the original licensed image unchanged as the source of record.
2. Record its model, version, byte count, and local SHA-256 in the private
   build record.
3. Split only after confirming the image's documented memory map and order.
   For a verified 16 KiB C/E image, use the page splitter's `CE` mode. For a
   documented 24 KiB C/A/E image, use `CAE`. Do not infer the layout from a
   filename.
4. Compose the private pages with the matching OneROM template and verify
   each page is 8192 bytes.
5. Test DOS identification, directory, load, save, reset, and the host KERNAL
   pairing separately.

This guide does not describe how to manufacture or distribute a JiffyDOS
image. A lawful license for the source is required.

## Experimental cable/RAMBoard patch notes

The project previously tested separate labels for a JiffyDOS “cable-safe”
variant and a RAMBoard variant. The cable candidate changed three bytes at
`$FF10` (`8E 03 18` to `EA EA EA`); it failed the Bruce Lee load test and is
not a parallel transfer protocol or a validated patch. Do not use it as a
release profile.

The [RAMBoardII repository](https://github.com/ytmytm/1541-RAMBOardII)
contains a JiffyDOS RAM patch for a different RAMBoard implementation. Its
license and hardware/base-ROM assumptions do not establish that it can be
reused on the original CLD RAMBoard and longboard 1541. This repository does
not include that patch source or claim compatibility. JiffyDOS remains a
serial protocol; the matching licensed C64 KERNAL is used for its serial fast
path.
