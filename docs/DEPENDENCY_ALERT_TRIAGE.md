# Dependency-alert triage

GitHub reports dependency severity for the affected package, not a verdict that
the package is reachable in a released Fire-24G image. This record separates
the advisory from the 1541HUD / OneROM exposure. Do not dismiss an alert merely
because its current project exposure is limited.

## Release baseline

This triage applies to `v8.1.0-fire24g-r1`, which vendors a pinned, unreleased
OneROM V8.1 development snapshot. The public release contains two firmware
images; neither image embeds the Rust host CLI or its archive parser.

## Open alerts

| Alert | Dependency and location | Project exposure | Decision |
| --- | --- | --- | --- |
| #25 - ZIP parser differential / ZIP cloaking | `zip` through `source/one-rom-v0.8.1/rust/fw/Cargo.toml`; workspace lock resolves `zip 6.0.0`. The CLI separately resolves `zip 8.6.0`. | **Host-build only.** `onerom-fw` opens a ZIP in memory, selects one configured member with `by_name`, and copies it into a memory buffer. It does not call the ZIP crate's path-extraction API or write archive-selected paths to disk. Public UB3/UB4 build manifests use local tracked/owner-supplied inputs rather than remote ZIP sources. A builder can still be exposed if they deliberately build from an untrusted ZIP source. | **Keep open.** Upgrade to a patched ZIP release only as a tracked upstream-dependency overlay, then complete a clean WSL2/Linux build and hardware qualification before publishing a replacement image. |
| #11 - Rustls TLS 1.3 handshake encryption-level boundary | `source/one-rom-v0.8.1/rust/Cargo.lock`; four Rustls advisories share this workspace lockfile. | **Host tooling only.** The qualified Control/Monitor build paths use `reqwest` with Rustls for outbound HTTPS when a user elects to retrieve remote material. The advisory describes a TLS 1.3 server-handshake condition. The vendored upstream `signing-server` can act as a TLS server, but it is not part of the qualified 1541HUD build path or either released firmware image. | **Keep open.** Update the workspace lockfile through a reviewed upstream-compatible Rustls/Reqwest resolution, then complete clean build and regression validation. Do not mark the installed UB3/UB4 firmware as affected. |

## Rules for classifying remaining alerts

1. **Released image runtime:** a dependency compiled into UB3/UB4 firmware and
   reachable from installed-board input has highest priority.
2. **Host build / CLI:** a dependency used only when composing, inspecting, or
   flashing images is a builder-workstation risk. Document the untrusted input
   condition and plan a qualified update.
3. **Prototype, test, or development-only tree:** do not claim it affects a
   release image. Keep it visible until its path and use are recorded; remove
   or update it only when doing so does not alter the pinned source boundary.
4. **Vendored upstream:** do not blindly apply Dependabot's update suggestion.
   Record the upstream revision/change, Rust toolchain impact, build result,
   package validation result, and hardware acceptance result.

## Builder guidance until patched

- Use only local ROM pages and known inputs you are entitled to use.
- Do not point a public build manifest at an untrusted ZIP URL or archive.
- Verify a firmware image with `onerom firmware inspect` and SHA-256 before
  flashing.
- Treat the bundled stock release images and their documented hashes as the
  supported public test baseline.

## Required evidence before closing an alert

- Exact advisory/CVE and affected path.
- Confirmation of whether the released image, build wrapper, or only an
  excluded prototype/test tree reaches the dependency.
- Patch/upgrade commit and updated lockfile.
- Clean package-validator result and relevant automated tests.
- Fresh source-build record; for firmware-impacting changes, hardware
  acceptance results too.

