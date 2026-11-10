#include <assert.h>
#include <stdio.h>
#include "monitor_rpm_validity.h"
int main(void) {
 uint8_t valid = 1, header_state = 4;
 uint16_t intervals = 73;
 hud_rpm_invalidate(&valid, &header_state, &intervals);
 assert(!valid && !header_state && !intervals);
 hud_rpm_invalidate(&valid, &header_state, &intervals);
 assert(!valid && !header_state && !intervals);
 for (unsigned bits = 0; bits < 32; ++bits) {
  unsigned checksum = bits & 1, known = (bits >> 1) & 1;
  unsigned motor = (bits >> 2) & 1, stepping = (bits >> 3) & 1;
  unsigned writing = (bits >> 4) & 1;
  assert(hud_rpm_header_eligible(checksum, known, motor, stepping, writing) ==
         (checksum && known && motor && !stepping && !writing));
 }
 puts("RPM validity: invalidation and header acceptance states passed");
}
