# Stock UB3 two-ROM drive test: 2026-11-10

The user tested the exact public stock UB3 Control image in a standard
two-ROM 1541 setup. The drive booted standard stock DOS, loaded a game, and
operated normally.

## Confirmed results

- IEC address and ROM selection settings saved.
- Write protection worked.
- Drive reset worked.
- Monitor and Control firmware operated correctly on the drive.

Known cosmetic reporting issue: an invalid IEC-address reply can display
`MAX=11` even though the production validator only accepts addresses 8 and 9.
The successful valid-address save test is unaffected; details remain in the
earlier loose-board record.

Image: `releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-2rom-v0.8.1.bin`

SHA-256: `A066F70C1B9636714C0D0A0694D4B981641DF3E1DDD024D0E210097697BCBAFD`

This is user-reported hardware acceptance; no serial number or private drive
ROM collection is included. The full USB disconnect/reconnect power-cycle
result was not reported. Earlier loose-board checks for the Control firmware
are retained in `STOCK_CONTROL_LOOSE_TEST_2026-11-10.md` and its machine-readable
record.
