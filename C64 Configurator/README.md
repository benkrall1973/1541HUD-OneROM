# C64 OneROM Configurator

This folder contains the V1.4.31 public baseline for the Commodore 64 BASIC
OneROM selector, its ASCII BASIC source, the small Python tokenizer used to
produce the PRG, and the reproducible release artifact. The utility reads the
catalog already installed on OneROM; it does not contain or install drive-ROM
images.

See the full [technical guide](<Technical Guide.md>) for
startup, controls, socket discovery, IEC address behavior, compatibility,
troubleshooting, and validation limits.

## Build and verify

Requirements: Windows PowerShell and Python 3. From the repository root, run:

```powershell
& "./C64 Configurator/Build-C64-OneROM-Configurator.ps1"
```

The script creates an ignored `build/` directory, tokenizes the tracked BASIC
source, and checks the output hash and size against `manifest.json` and the
tracked PRG. It does not modify the published release file. The tokenizer uses
only the Python standard library. Source and tokenizer checksums use LF-normalized
UTF-8 text, so Git checkout line endings do not change the recorded hashes.

The project-authored BASIC source, tokenizer, build wrapper, and documentation
are covered by the repository-root MIT license. No DOS ROM, C64 KERNAL,
JiffyDOS payload, or generated S-JiffyDOS image is included.
