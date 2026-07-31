#ifndef IPV4_H
#define IPV4_H

#include "ipv4header.h"
#include <stdio.h>

typedef struct {
    ipv4header_t header;
    uint8_t *data;
    size_t data_length;
} ipv4_t;

void ipv4_init(ipv4_t *ipv4);
void ipv4_build_bytes(uint8_t *buffer, ipv4_t *ipv4);
void ipv4_from_bytes(ipv4_t *ipv4, uint8_t *buffer);
void ipv4_print_dec(ipv4_t *ipv4);
void ipv4_print_hex(ipv4_t *ipv4);
void ipv4_print_bin(ipv4_t *ipv4);

/* Header Checksum */
uint16_t ipv4_get_16bit(const ipv4header_t *header, uint8_t index);
uint16_t ipv4_compute_header_checksum(ipv4_t *ipv4);

#endif
