# Stock Control loose-board acceptance: 2026-11-10

The exact public Fire-24G stock Control image was flashed to a loose OneROM
with the patched Windows CLI 0.6.0-hud1. Programming verification passed.
USB returned in running mode, the single stock slot was active, and a normal
reboot succeeded. A pre-flash backup and serial-specific records are archived
privately. No image was rebuilt or changed for this test.

Historical image, since replaced in the public package:
`onerom-1541HUD-Control-UB3-fire-24-g-stock-test-v0.8.1.bin`

SHA-256: 734736EB8E2E990FFD101A3918E8834DBAA10F8173A1B876DDAD4AC75E378D40

## Results

All 47 final automated checks passed. The public machine-readable record is
[stock-control-loose-2026-11-10.json](../qualification/stock-control-loose-2026-11-10.json).

- CDC API readiness, stock slot count and label, and active slot inspection.
- Rejection of invalid/unavailable ROM slots and IEC addresses outside 8/9,
  with saved settings unchanged by rejected commands.
- Saving IEC 9 and LEDs OFF, rebooting through the same CLI control command
  used by HUD, and confirming both saved settings afterward with USB power
  continuously connected.
- GPIO26 released to input for IEC 9, then driven low for restored IEC 8.
  The loose board has no drive pull-up, so a high input level is not expected.
- WP override ON/OFF replies and GPIO25 high/low readback.
- Reset start/release replies and GPIO27 returned to input afterward.
- Twelve repeated CDC queries without communication failure.
- Starting IEC 8 and AUTO LED policy restored and confirmed after another
  reboot; WP override OFF and stock slot active at completion.

This test exercised the persistent `ROMIEC` save and CLI OneROM reboot path on
a loose board. It did not test the V0.8.0.53 `ROMIEC_NOW` Apply Only behavior
or whether a VBUS-powered UB3 loses power when the 1541 is switched off.

An initial test incorrectly expected GPIO26 to drive high for IEC 9. That
assertion was corrected against the production implementation and the full
test rerun. Releasing the pin to input is the intended firmware behavior.

## Known reporting defect

An invalid IEC-address reply from this historical image prints MIN=8,MAX=11
even though the production validator correctly accepts only 8 and 9. SEL_B is
allocated to reset in this profile, so IEC 10/11 are not supported. A later
source-only correction changes the diagnostic to MAX=9; neither this tested
image nor the published .16/.17 images were rebuilt for that correction. The
error text does not change address validation or the recorded test results.

## Historical image scope

This record applies only to the earlier one-slot image with hash
`734736EB8E2E990FFD101A3918E8834DBAA10F8173A1B876DDAD4AC75E378D40`.
It is retained as history and is not the current public stock image. The
subsequent two-slot stock DOS 2.6 image was tested in the drive; see
[`STOCK_CONTROL_DRIVE_TEST_2026-11-10.md`](STOCK_CONTROL_DRIVE_TEST_2026-11-10.md).

The earlier loose-board test did not validate:

- Actual stock DOS startup, directory reads, or ROM operation in a 1541.
- IEC bus communication at addresses 8/9.
- Electrical reset waveform/timing through the C46 circuit.
- Physical write-protect sensing or override with the drive connected.
- Persistence through a complete USB power cycle.
- Alternate ROM selection: this stock image has one fixed slot and no
  selectable replacement DOS profiles.

The subsequent two-ROM test does not retroactively change these earlier
loose-board results or their image hash.
