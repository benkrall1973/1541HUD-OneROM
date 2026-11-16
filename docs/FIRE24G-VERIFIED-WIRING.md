# Fire-24G wiring for a 1541 long board

Use this guide with the matching UB3 Control image. These connections were
verified on the documented 1541 long-board setup; other drive revisions need
their own wiring review. Disconnect power before wiring.

## First choose the ROM wiring

OneROM can serve two ROM pages directly, or three pages through a 74LS138.
Choose the row that matches the hardware already in your drive.

| Drive setup | OneROM placement | X1 | X2 |
| --- | --- | --- | --- |
| Two ROMs; no 74LS138 | Fit OneROM in the lower `$C000` socket (mainboard UB3) | Connect to pin 20 (`/CS`) of the empty upper `$E000` socket (mainboard UB4). | Leave unused. |
| Two ROMs; OneROM in upper socket | Fit OneROM in the upper `$E000` socket (mainboard UB4) | Connect to pin 20 (`/CS`) of the empty lower `$C000` socket (mainboard UB3). | Leave unused. |
| Three pages; 74LS138 installed | Use the documented C/A/E decoder wiring | 74LS138 pin 10, `/Y5` (A page) | 74LS138 pin 7, `/Y7` (E page) |

The labels UB3 and UB4 in the two-ROM rows are **1541 mainboard socket names**.
They are unrelated to the project names UB3 Control and UB4 Monitor. For the
three-page setup, use the C/A/E composition template and keep the pages in
C, A, E order. Do not connect X1 and X2 to the decoder outputs for a direct
two-page setup.

## UB3 Control connections

| Signal | GPIO | Connect to | Notes |
| --- | ---: | --- | --- |
| `SEL_A` | 26 | Existing J1 IEC address-control wire | LOW selects device 8; released selects device 9. |
| `SEL_B` | 27 | C46 positive / CR7 anode through **1 kOhm** | Active-low reset. Never connect directly to UC4 pin 40. |
| `SEL_C` | 25 | KSC2383YTA transistor base through **5.1 kOhm** | Optional write-protect override. |
| `SEL_D` | 24 | P6 pin 12 through **22 kOhm** | Optional write-protect sense; add another 22 kOhm from SEL_D to ground. |
| `X1`, `X2` | 9, 8 | Select one ROM wiring option above | These signals are not interchangeable between layouts. |

### Reset

The 1 kOhm series resistor is required. The earlier 2.2 kOhm value did not
pull C46 low enough and could leave IEC access stalled after reset. The
firmware holds reset low for 350 ms, then releases the pin. It never drives
`SEL_B` high. The red LED and motor may run during that short reset pulse.

### Optional write-protect circuit

For override, connect a **KSC2383YTA NPN** emitter to drive ground, collector
to P6 pin 12, and base through 5.1 kOhm to `SEL_C`. Do not connect a OneROM
GPIO directly to P6 pin 12. The specified transistor uses pin 1 emitter,
pin 2 collector, pin 3 base; confirm the datasheet's view before assembly.

For sensing, connect P6 pin 12 through 22 kOhm to `SEL_D`, then add 22 kOhm
from `SEL_D` to ground. `SEL_D` is input-only. When override is on, the
transistor pulls the shared WP node low, so physical disk WP status is masked.

## UB4 Monitor connections

UB4 is passive telemetry only. It does not serve ROMs, reset the drive, or
control write protect.

| Signal | GPIO | Connect to |
| --- | ---: | --- |
| `SEL_A` | 26 | UC2 pin 23 (`/CS2`) |
| `SEL_B` | 27 | UC2 pin 17 (PB7 / active-low SYNC) |
| `X1` | 9 | UC4 pin 34 (`R/W`) |
| `X2` | 8 | UC4 pin 39 (`phi2`) |

## USB power and Apply Only

Apply Only changes the live IEC address but does not save it to NV2. A normal
USB cable can keep UB3 powered while the drive is off, so a drive-only power
cycle may leave the temporary address selected. To reload the saved address
while leaving USB data connected, use a data-capable cable/adapter that blocks
host VBUS +5 V and still passes D+/D− and ground. Or power-cycle UB3 too.

Apply+Save stores the address in NV2 and uses the controlled OneROM reboot.
Automatic drive power-off sensing is not implemented.

## Quick check after wiring

1. Confirm that the ROM template matches the physical wiring.
2. Boot the drive and check its normal device address.
3. Use 1541HUD to test reset, then immediately read the directory.
4. If fitted, check write-protect sensing and override separately.

For the electrical measurements and full verification record, see the
[engineering dossier](FIRE24G_V81_ENGINEERING_DOSSIER.md).
