#ifndef IPV4_OPTION_H
#define IPV4_OPTION_H

#include "security.h"
#include "source_route.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  uint8_t nb;
  uint8_t option_lengths[40];
  uint8_t length;
  uint8_t buffer[40];
} ipv4option_t;

void ipv4option_init(ipv4option_t *option);

uint8_t ipv4option_push_no_option(ipv4option_t *option);
uint8_t ipv4option_push_end_option(ipv4option_t *option);
uint8_t ipv4option_push_security(ipv4option_t *option,
                                 ipv4option_security_t *security);
uint8_t ipv4option_push_sr(ipv4option_t *option, ipv4option_sr_t *sr);
uint8_t ipv4option_push_stream_id(ipv4option_t *option, uint16_t stream_id);
uint8_t ipv4option_push_internet_timestamp(ipv4option_t *option, uint8_t flag);
uint8_t ipv4option_push_route_alert(ipv4option_t *option);
uint8_t ipv4option_pop_option(ipv4option_t *option);

uint8_t ipv4option_from_bytes(ipv4option_t *option, uint8_t *buffer,
                              uint8_t length);
void ipv4option_to_bytes(uint8_t *buffer, ipv4option_t *option);

#endif
