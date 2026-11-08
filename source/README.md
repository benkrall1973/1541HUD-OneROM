# Official OneROM V8.1 source

`one-rom-v0.8.1/` is the single tracked upstream-source baseline for this
repository. It is the exact signed upstream `v0.8.1` release tag
(`a21dcbf8e87a74f56c03b23ecdd4055192c7655e`) from Piers Finlayson's One ROM
repository.

It is intentionally stored in this repository so a clean clone has the exact
source required for the qualified build. Do not replace it with a live upstream
pull without an explicit review, integration, build, and hardware-test pass.

Both UB3 Control and UB4 Monitor wrappers build directly from this tree. UB3's
1541-specific behaviour is provided by its custom USB and Host-Control plugins;
the core remains the official v0.8.1 release.
