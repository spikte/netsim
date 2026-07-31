#ifndef SECURITY_H
#define SECURITY_H

#include <stdint.h>

enum ipv4OptionSecurity {
  Unclassified = 0x0,
  Confidential = 0xF135,
  EFTO = 0x789A,
  MMMM = 0xBC4D,
  PROG = 0x5E26,
  Restricted = 0xAF13,
  Secret = 0xD788,
  TopSecret = 0x6BC5
};

typedef struct {                      // True size (in bits)
  uint16_t security;                  // 16
  uint16_t compartments;              // 16
  uint16_t handling_restrictions;     // 16
  uint32_t transmission_control_code; // 24
} ipv4option_security_t;

void ipv4option_security_init(ipv4option_security_t *option);

/* Security */
void ipv4option_security_set_security(ipv4option_security_t *option,
                                      enum ipv4OptionSecurity security);

/* Compartments */
// TODO: Find documentation

/* Handling Restrictions */
// TODO: Find documentation

/* Transmission Control Code */
// TODO: Find documentation

#endif
