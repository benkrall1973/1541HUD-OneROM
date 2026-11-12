# Stock UB3 two-ROM drive test: 2026-11-10

The user tested the original public stock UB3 Control build in a standard
two-ROM 1541 setup. The drive booted standard stock DOS, loaded a game, and
operated normally. Before publication, two equal-length NUL-terminated TinyUSB
diagnostic path strings were normalized to remove a local build path. The
published artifact was not reflashed after this diagnostic-only change.

## Confirmed results

- IEC address and ROM selection settings saved.
- Write protection worked.
- Drive reset worked.
- Monitor and Control firmware operated correctly on the drive.

The tested image's invalid IEC-address reply can display `MAX=11` even though
the validator only accepts addresses 8 and 9. A later source-only correction
changes the diagnostic to `MAX=9`; the tested and published image was not
rebuilt for it. The valid-address save test is unaffected; details remain in
the earlier loose-board record.

Image: `releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-2rom-v0.8.1.bin`

SHA-256: `DB2A9F85E58A54341FB5C6572252069E4F8414717406A17DA294BB0E8A69B842`

This is user-reported hardware acceptance; no serial number or private drive
ROM collection is included. The full USB disconnect/reconnect power-cycle
result was not reported. Earlier loose-board checks for the Control firmware
are retained in `STOCK_CONTROL_LOOSE_TEST_2026-11-10.md` and its machine-readable
record.

This stock-image test confirms saved IEC/ROM settings and normal drive
operation; it does not qualify the later V0.8.0.53 Apply Only candidate. That
candidate leaves the live SEL_A address in place while UB3 remains powered. To
restore saved NV2 after cycling only the 1541 while keeping USB data connected,
use a cable with host VBUS +5 V blocked, or power-cycle UB3 explicitly.
