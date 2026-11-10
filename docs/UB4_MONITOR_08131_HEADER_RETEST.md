# Monitor 0.8.1.31 header-checksum test candidate

Historical .31 retest record: the supplied drive log showed normal operation
with R=0 Q=0 and stable RPM. The current .32 release retains this checksum repair.
Monitor USB stays at 0.8.1.4; the mailbox ABI and V51 record formats are unchanged.

## Repair

The decoder previously emitted HDRPHY after a complete physical-header RAM
sequence even when its XOR checksum failed. V51 uses HDRPHY for recent-sector
history and coverage, so those displays could count a corrupt header. RPM
already required checksum validity and was not affected by this reporting gap.

HDRPHY now requires a valid checksum. Sampled HDRMETA diagnostics, including
OK=0 for failures, retain their existing bounded rate. Checksum-invalid headers
no longer become coverage events. PIO/DMA capture, timing, RPM calculations,
NV handling, GPIO wiring and Control firmware are unchanged.

## Host evidence

test_monitor_headers.c includes the production decoder and supplies real GPIO
snapshot encodings for the physical-header RAM write sequence. Test-only guards
replace the timer/barrier and omit the hardware entry point; normal firmware
does not enable these guards. Tests cover all single-bit checksum corruptions,
valid-header recovery, metadata sampling, disabled subscriptions and the DOS
pre-search write order. Address/undefined-behavior sanitizer checks passed.

The same regression test was run against a temporary copy with the checksum
gate removed. It failed on the corrupt-header coverage assertion, demonstrating
that it catches the previous behavior rather than testing a copied decoder.

## Drive retest

1. Back up the intended Monitor by serial before flashing. Preserve its NV
   sector; never select the working UB3 Control board.
2. Confirm Monitor 0.8.1.31 and USB 0.8.1.4, then connect V51.
3. Read a known-good disk. Check sector history/coverage still populate and
   valid header metadata remains OK=1. Coverage measures observed sectors;
   normal DOS activity need not encounter every sector.
4. Repeat seeking/homing and reads across density zones. Check RPM returns
   after fresh headers and does not reproduce the stale-zone spike.
5. Confirm R/Q do not grow unexpectedly and disk removal/WP feedback still
   behave normally. Preserve the complete log.
6. If an existing expendable disk already produces bad headers, confirm
   sampled OK=0 diagnostics remain visible. Do not damage or write a disk
   merely to generate errors. Host tests establish rejection of corrupt headers;
   a clean-disk hardware log alone does not exercise that path.

The candidate has not been flashed automatically. Publish its new image and
update release manifests only after the user completes the drive retest.
