# Fire-24G V8.1 Engineering Dossier

> **Upstream status:** The base is official OneROM v0.8.1, commit
> `1b189260d85a5feced19e969399986f5f1d1cabe`. Custom 1541HUD plugins and
> role-specific overlays implement the community-tested Control and Monitor
> integration. See V81_BASELINE_RUNBOOK.md for test scope.

This is the build-critical contract for the qualified 1541HUD system:
Fire-24G OneROM UB3 Control, Fire-24G OneROM UB4 Monitor, and the Windows
1541HUD. It supplements the detailed build/runbook documents. If a historical
summary disagrees with this file, source and the qualified build manifest win.

## Qualified scope

- **UB3 Control:** ROM selection, IEC 8/9, reset, write-protect control and
  sense, persisted LED policy.
- **UB4 Monitor:** electrically passive 1541 capture, SYNC/RPM telemetry,
  density/activity evidence, persisted LED policy.
- **HUD:** two independent USB CDC roles bound by OneROM USB serial—not the
  Windows COM-number lottery.

UB3 and UB4 are different source compositions, firmware images, and wiring.
Never exchange them.

## Fire-24G GPIO contract

### UB3 Control

| Signal | GPIO | 1541 endpoint | Rule |
| --- | ---: | --- | --- |
| SEL_A | 26 | J1 IEC address control | LOW = device 8; released/input = device 9 |
| SEL_B | 27 | C46 positive / CR7 anode through **1 kOhm** | reset only; LOW then input, never HIGH |
| SEL_C | 25 | 5.1 kOhm to KSC2383YTA base | write-protect override |
| SEL_D | 24 | P6 pin 12 through 22k/22k divider | write-protect sense |
| X1 | 9 | 74LS138 pin 10 `/Y5` | `$A000-$BFFF` page select |
| X2 | 8 | 74LS138 pin 7 `/Y7` | `$E000-$FFFF` page select |

The reset assertion is **350 ms**, followed by release to input. The 1 kOhm
series resistor is not optional: the former 2.2 kOhm value was associated with
post-reset IEC directory stalls. HUD's roughly seven-second reset feedback is
recovery display time, not a seven-second electrical assertion.

UB3 reserves only `sel_b`, `sel_c`, and `sel_d` from V8.1 image selection.
`sel_a`, `x1`, and `x2` are live functions and must remain available.

### UB4 Monitor

| Signal | GPIO | 1541 endpoint | Purpose |
| --- | ---: | --- | --- |
| SEL_A | 26 | UC2 pin 23 `/CS2` | VIA2 select capture |
| SEL_B | 27 | UC2 pin 17 PB7 active-low `SYNC` | raw-SYNC/RPM capture |
| X1 | 9 | UC4 pin 34 `R/W` | write-cycle qualification |
| X2 | 8 | UC4 pin 39 `phi2` | bus timing capture |

UB4 has no write-protect wire or authority. It is passive monitoring only.

## Three-page drive-ROM decode

UB3 native `/CS` selects `$C000-$DFFF`. A separate SN74LS138 supplies the
other two pages: `/Y5` to X1 for `$A000-$BFFF`, and `/Y7` to X2 for
`$E000-$FFFF`. Multi-page images are C, A, E in that order.

| 74LS138 pin | Connection |
| --- | --- |
| 16 / 8 | +5 V / ground; fit a 0.1 uF bypass capacitor directly across them |
| 1, 2, 3 | UC4 pins 23, 24, 25 (A13, A14, A15) |
| 4, 5 / 6 | ground / +5 V |
| 10 / 7 | UB3 X1 `/Y5` A page / UB3 X2 `/Y7` E page |

The old UB4 pin-20 select wire is unrelated and stays disconnected/insulated.

## USB, overlays, and persistence

1. Build separate Control and Monitor overlays from pinned upstream V8.1.
   Do not merge their compositions.
2. UB4 must be built with `HUD1541_PASSIVE_UB4` in the core. Merely disabling
   a plugin does not make generic ROM PIO/DMA ownership passive.
3. UB3 CDC requires exclusive serial input and queued/full response output;
   boot logs must not consume the selector-response FIFO.
4. Keep UB3's cooperative `ora_yield(NULL)` idle path. Without it, Control NV
   writes can fail despite apparently healthy USB communication.
5. Control NV allocation: NV0 selected ROM, NV1 LED policy, NV2 IEC address.
   Monitor owns its NV1 LED policy separately.
6. UB4 may change its LED state immediately, but defers an NV1 save until the
   motor is off and capture is idle. HUD must report this as pending—not force
   a write during a disk read.

## Front-panel LEDs

Fire-24G maps logical status and RGB channels to the same physical LED GPIO.
The user policy is therefore paired: `AUTO` enables the status/RGB behavior,
while `OFF` explicitly disables both. UB3 and UB4 store independent policies.

### UB3 selected-ROM colours

| Slot | Profile | Colour |
| ---: | --- | --- |
| 0 | bootloader/default | blue |
| 1 | DOS 2.6 | cyan |
| 2 | 1541-II | purple |
| 3 | JiffyDOS | magenta |
| 4 | JiffyDOS parallel | amber |
| 5 | RAMBoard | lime |
| 6 | DolphinDOS | teal |
| 7 | SpeedDOS | white |
| 8 | SpeedDOS RAMBoard | orange |

Bounded UB3 feedback: ROM save white breathe (350 ms); IEC 8 blue blink
(500 ms); IEC 9 purple blink (740 ms); reset amber breathe (seven-second HUD
recovery display).

### UB4 monitor colours

| State | Colour |
| --- | --- |
| capture setup | purple |
| armed/waiting | green |
| density D3, tracks 1–17 | yellow |
| density D2, tracks 18–24 | blue |
| density D1, tracks 25–30 | orange |
| density D0, tracks 31–35 | white |
| recent overflow | blinking amber, 1.5-second hold after the last overflow |
| unsafe GPIO, PIO/DMA failure, or resource conflict | fast blinking red |

These operational states override density. When no HUD CDC client is attached,
events are intentionally discarded instead of accumulating into a false amber
warning.

## Acceptance requirements

- **UB3:** ROM/IEC save and readback; write-protect OFF/ON/OFF; `ROMRESET`;
  then immediate `LOAD"$",8`/directory access without a C64 stall.
- **UB4:** green idle, density indication under real disk activity, valid
  raw-SYNC RPM, and no influence on drive operation.
- **HUD:** bind roles by USB serial, release ports before flashing, preserve
  the viewed card after actions, and explain deferred Monitor LED saves.
