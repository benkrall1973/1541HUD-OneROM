# Security and safety reporting

## Security issues

Do not publish credential material, private serial numbers, or a vulnerability
that could expose a user's machine or account. Open a private GitHub security
advisory for this repository where available, or contact the repository owner
through GitHub before posting public details.

## Hardware safety issues

Treat unsafe wiring, unexpected GPIO drive behavior, reset-line faults,
unintended power reaching 1541 rails or signals, and firmware that can damage
a 1541 as high-priority safety issues. Normal host USB +5 V powering the
Fire-24G UB3 through its USB port is an intended operating condition; its
possible effect on Apply Only IEC-address restoration is documented in the
README. Do not encourage others to reproduce an actual unsafe wiring condition
until it has been reviewed. Include board revision, firmware image/hash,
wiring, power state, and a minimally sufficient reproduction.

This project is a hobbyist/community integration, not safety-certified
equipment. Disconnect drive power before changing wiring.

