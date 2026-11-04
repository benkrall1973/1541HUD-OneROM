# Fire-24G verified wiring and reset behavior

This document records the hardware configuration verified on the 1541 long
board using the Fire-24G OneROM image on the public `v8.1.0-fire24g` branch
(currently identical to `main`).

## Selector mapping

| Function | Fire-24G header signal | RP2350 GPIO | Wiring |
| --- | --- | ---: | --- |
| IEC device 8/9 | `SEL_A` | 26 | Existing J1 address-control connection; LOW selects device 8, released selects device 9. |
| 1541 reset | `SEL_B` | 27 | Through the 1 kOhm resistor described below. |
| Write-protect override | `SEL_C` | 25 | Through 5.1 kOhm to the override NPN base. |
| Write-protect sense | `SEL_D` | 24 | 22 kOhm / 22 kOhm divider from 1541 P6 pin 12. |
| A-page select | `X1` | 9 | 74LS138 `/Y5`. |
| E-page select | `X2` | 8 | 74LS138 `/Y7`. |

`SEL_A` and `SEL_B` are ADC-class GPIOs according to the OneROM CLI. The
existing installation uses SEL_B only as an active-low, open-drain-style reset
control: firmware never drives it HIGH.

## C46 reset path

Connect Fire-24G `SEL_B` to 1541 **C46 positive** (or the equivalent CR7
anode reset node) through a **1 kOhm series resistor**. Do not connect it to
UC4 pin 40, which is driven by the 1541 reset logic.

The prior 2.2 kOhm resistor left C46 at about 1.2 V during a ten-second reset
test. Replacing it with 1 kOhm lowered C46 to about 0.8 V. This corrected the
failure where the C64 remained alive but `LOAD"$",8` waited indefinitely after
a HUD reset. Immediate directory access after reset was hardware-verified.

The final firmware behavior is:

- Manual `ROMRESET`: drive SEL_B LOW for 350 ms, then return it to input.
- OneROM boot/reboot: assert SEL_B from early core GPIO setup through a 350 ms
  plugin hold, then return it to input only after ROM serving is available.
- SEL_B is never driven HIGH.

During a manual reset the 1541 red LED and motor can be active for the
350 ms reset hold. That is expected.

## Reset electrical estimate

C46 is 100 uF. With the verified 1 kOhm resistor, the initial capacitor
discharge current is approximately 3.9–5 mA and decays with an approximately
100 ms time constant. After the capacitor is discharged, the 1541 pull-up path
supplies roughly 0.5–0.7 mA. This reset is used infrequently and is not a
meaningful thermal load on the OneROM.

## Acceptance test

1. Boot the C64 and 1541 normally at device 8.
2. In 1541HUD, press **Reset 1541**.
3. Wait for the reset card to report that the pulse was released.
4. Immediately issue `LOAD"$",8` (or the JiffyDOS directory command).
5. Successful immediate directory access confirms a complete reset and IEC
   recovery.
