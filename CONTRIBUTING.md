# Contributing to 1541HUD / OneROM Fire-24G

Thanks for helping improve the project. This is hardware-adjacent firmware:
please make changes reviewable, reversible, and boring in the best possible
way.

## Before opening a change

1. Start from the current `v8.1` branch and use a focused branch.
2. Do not include ROMs, ROM-derived images, personal manifests, device serials,
   local paths, disk images, or generated build output.
3. Read the wiring and safety constraints before changing GPIO, reset,
   write-protect, or ROM-serving code.
4. Describe the tested hardware, board role, wiring, toolchain, and acceptance
   result in the pull request.

## Required checks

Run the public-package validator from the repository root:

```powershell
./tools/maintenance/Test-Public-Package.ps1 -PackageRoot .
```

Run relevant 1541HUD tests and follow the WSL2/Linux route for firmware source
builds. A successful compile is not proof that hardware behavior is safe:
state exactly what was tested and what was not.

## Changes affecting hardware

GPIO and reset changes require a documented electrical rationale and a
hardware acceptance sequence. Do not describe an untested wiring change as
verified. Keep Fire-24G-specific assumptions explicit.

## License and provenance

By contributing, you agree that your original contribution may be distributed
under the repository's MIT License. Preserve upstream and dependency notices.

