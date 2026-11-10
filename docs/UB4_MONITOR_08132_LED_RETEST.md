# Monitor 0.8.1.32 / USB 0.8.1.5 LED persistence candidate

Status: build/host checks passed; user-confirmed drive tests accepted. Control
firmware and the V51 application are unchanged. The image includes the .31
checksum gate already exercised in the drive retest.

## Repair

LED set/query replies now use the current request sequence, completed sequence
and persistence result. Pending requests return PENDING/SAVED=0 with the
motor-stop or capture-idle reason. Completed failures return FAIL/SAVED=0.
SAVED=1 requires the current request's successful completion, or the initial
boot-restored policy with no outstanding request.

An acknowledgement for an earlier request is withheld while a newer request
is pending. Request numbering comes from the mailbox and avoids zero and an
already-applied sequence during wrap. The capture plugin publishes its result
through a memory barrier before publishing the completion sequence. The ABI
and V51 reply format stay unchanged; no drive bus wiring is changed.

## Tests

The native test includes production monitor_transport.c and the actual parser
and output queue. It uses real OneROM API types with a stubbed USB I/O boundary
and an in-memory mailbox. Tests exercise boot queries, motor/capture waiting,
failure, result-before-completion, superseded acknowledgements, sequence wrap,
output backpressure, reconnect and single acknowledgement delivery. Sanitizer
checks passed. Three additional V51 tests confirm pending replies retain the
save/health hold, successful saves rebaseline health, and failures are rejected.
This does not test actual flash failure or cross-core hardware timing.

## Drive retest

1. Back up and identify the Monitor before flashing; preserve NV. Confirm
   Monitor 0.8.1.32 and USB 0.8.1.5 afterward. Do not program Control.
2. Read a disk, then toggle Monitor LED policy while the motor is running.
   The LED can change immediately; the save should remain pending until idle.
3. If a query/refresh occurs while pending, expect PENDING/SAVED=0 rather than
   a false saved confirmation. Use the HUD connection; do not open its occupied
   serial port in a second program.
4. Allow the motor to stop and capture to drain. Expect an OK/SAVED=1 reply.
   Repeat with the other policy and confirm capture/health reporting resumes.
5. At idle, reboot or power-cycle the Monitor and confirm the last saved
   policy returns. A drive-only reset is not a Monitor reboot when USB keeps
   the board powered.
6. Exercise normal reads/seeks, sector coverage and RPM. Check R/Q and retain
   the full log. Quick successive policy changes should settle on the latest
   requested policy without an earlier save confirming the wrong mode.

Do not interrupt a flash write or damage hardware to induce failure. The
failure-response path is covered by the host test. The candidate is not
flashed without explicit authorization. The user subsequently confirmed LED
control during disk reads and persistence after a full power cycle. These are
user-reported hardware results; no .32 serial log was supplied. Host tests cover
the precise pending/failure/supersession reply cases.
