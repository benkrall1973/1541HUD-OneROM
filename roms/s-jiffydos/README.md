# S-JiffyDOS 1541

## What it is

S-JiffyDOS 1541 (SJD41) is a drive-side JiffyDOS patch. For this OneROM setup,
part of its running code lives in the RAMBoard's 8 KiB shadow-RAM area at
`$8000-$9FFF`. The RAMBoard provides that memory; the project startup routine
loads and verifies the code there automatically.

The generated S-JiffyDOS image is 32 KiB and is divided into four 8 KiB pages:

| Image offset | Use in this OneROM setup |
| --- | --- |
| `$0000-$1FFF` | Shadow code, staged in the `$A000-$BFFF` window for startup and copied into RAMBoard RAM at `$8000-$9FFF` |
| `$2000-$3FFF` | Runtime A page (`$A000-$BFFF`) |
| `$4000-$5FFF` | Runtime C page (`$C000-$DFFF`) |
| `$6000-$7FFF` | Runtime E page (`$E000-$FFFF`) |

The selector treats the runtime as a OneROM profile. Its C/A/E pages are
selected with the matching decoder wiring; the extra shadow page is used by
the startup profile before the runtime starts.

## What users need

- A lawful, matching JiffyDOS 1541 drive-ROM input. The project's tested
  reference used JiffyDOS 1541-II 6.00.
- A lawful copy of the S-JiffyDOS 1541 generator and its instructions. The
  generator disk and manual are third-party materials and are not included.
- OneROM C/A/E wiring and a working RAMBoard that exposes 8 KiB of RAM at
  `$8000-$9FFF`.
- For JiffyDOS fast serial transfers, a compatible JiffyDOS C64 KERNAL. SJD41
  and S-JiffyDOS 64 are separate; the special C64-side S-JiffyDOS KERNAL is
  not required just to run the drive patch. Ordinary IEC remains available.

JiffyDOS is commercial software. This repository does not include its drive
ROM, the S-JiffyDOS generator, generated S-JiffyDOS pages, or a ready-made
profile image. The root MIT license applies to project-authored integration
code and documentation; it does not grant rights to those third-party files.
Obtain and use them only under the applicable license.

## Build and startup integration

Use the instructions supplied with your lawful S-JiffyDOS generator to build
the complete 32 KiB output from the matching JiffyDOS input. Confirm the
output is exactly 32,768 bytes, then split it into the four consecutive 8 KiB
pages listed above. Do not substitute an unrelated ROM dump or guess at page
boundaries.

The project-authored OneROM startup source is included here:

- [Bootstrap assembly](source/one-rom-autostart/autostart.s)
- [Linker configuration](source/one-rom-autostart/autostart.cfg)
- [Build file](source/one-rom-autostart/Makefile)
- [Mock-bus bootstrap checks](source/one-rom-autostart/test_autostart.py)

The temporary Auto Start profile exposes the shadow page at `$A000-$BFFF` and
runs the bootstrap from its E page. The bootstrap copies all 8 KiB into the
RAMBoard window at `$8000-$9FFF` and verifies the copy. It then uses the OneROM
control protocol to load the runtime profile into an inactive OneROM RAM slot,
switch to that profile, and restart the drive. The assembly constant
`RBCP_RUNTIME_FLASH_SLOT` identifies the source OneROM flash slot; it defaults to
10 and can be overridden when assembling if the profile order differs.

To build the project-authored bootstrap, install `ca65`, `ld65`, and Python 3,
then run from the repository root:

```sh
make -C roms/s-jiffydos/source/one-rom-autostart
```

The Makefile writes its generated file under the ignored
`roms/private/s-jiffydos-autostart/` directory. The build produces the
bootstrap only; users still supply the licensed ROM input and generator, build
their four pages, and compose a profile for their own OneROM catalog and
wiring. `RUNTIME_FLASH_SLOT` must match the runtime profile's OneROM flash slot.

## RAMBoard warning

While S-JiffyDOS is active, its live code occupies RAMBoard RAM at
`$8000-$9FFF`. **Do not run software that writes to or uses this range**—RAM
tests, RAM expansion software, direct memory writes, or other programs can
overwrite the shadow code and corrupt the running DOS. A full drive power
cycle clears the volatile RAM; run the startup bootstrap again before using
the S-JiffyDOS profile.

This warning applies to this shadow-RAM S-JiffyDOS setup. It does not mean
every ordinary RAMBoard installation reserves `$8000-$9FFF` in software.
