#ifndef SOURCE_ROUTE_H
#define SOURCE_ROUTE_H

#include <stddef.h>
#include <stdint.h>

enum IPv4OptionSourceRoute { RR = 7, LSRR = 131, SSRR = 137 };

typedef struct {
  uint8_t type;
  uint8_t length;
  uint8_t pointer;
  uint32_t route[9];
  uint8_t route_length;
} ipv4option_sr_t;

void ipv4option_sr_init(ipv4option_sr_t *sr);
void ipv4option_sr_set_type(ipv4option_sr_t *sr,
                            enum IPv4OptionSourceRoute type);
void ipv4option_sr_push_address(ipv4option_sr_t *sr, uint32_t address);
uint32_t ipv4option_sr_pop_address(ipv4option_sr_t *sr);
void ipv4option_sr_set_address(ipv4option_sr_t *sr, uint8_t i,
                               uint32_t address);

#endif
