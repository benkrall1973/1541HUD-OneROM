#ifndef MONITOR_RPM_VALIDITY_H
#define MONITOR_RPM_VALIDITY_H
#include <stdint.h>

/* A seek/stop/capture gap destroys the relationship between SYNC and the
 * last decoded header. Never substitute the DOS destination as evidence. */
static inline void hud_rpm_invalidate(uint8_t *valid, uint8_t *header_state,
                                      uint16_t *intervals) {
 *valid = 0u;
 *header_state = 0u;
 *intervals = 0u;
}

static inline uint8_t hud_rpm_header_eligible(uint8_t checksum_ok,
 uint8_t motor_valid, uint8_t motor_on, uint8_t stepping, uint8_t writing) {
 return checksum_ok && motor_valid && motor_on && !stepping && !writing;
}
#endif
