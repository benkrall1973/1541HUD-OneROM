# Pinned OneROM V8.1 development snapshot

`one-rom-v0.8.1/` is the single tracked upstream-source baseline for this
repository. It was captured from Piers Finlayson's unreleased
[`releases/0.8.1` development branch](https://github.com/piersfinlayson/one-rom/tree/releases/0.8.1)
for the qualified Fire-24G work.

It is intentionally stored in this repository so a clean clone has the exact
source required for the qualified build. It is not an official upstream V8.1
release and must not be replaced by a live upstream pull without an explicit
review, integration, build, and hardware-test pass.

The UB4 Monitor wrapper builds directly from this tree. The UB3 Control wrapper
uses this same tree plus the tracked
`firmware-control/overlays/control-core-v0.8.1.patch` in an ignored work
directory, recreating the separate V8.1 snapshot against which Control was
qualified. This preserves reproducibility without carrying a second 100 MB
source checkout in the public tree.
