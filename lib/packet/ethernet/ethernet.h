#ifndef ETHERNET_H
#define ETHERNET_H

#include "ethernetheader.h"
#include "../crypto/crc.h"

typedef struct {
    ethernetheader_t header;
    uint8_t* payload;
    ethernettail_t tail;
    uint16_t payload_size;
} ethernet_t;

void ethernet_init(ethernet_t* ethernet);
size_t ethernet_size(ethernet_t* ethernet);
size_t ethernet_build_bytes(uint8_t *buffer, ethernet_t *ethernet, bool preamble);
uint8_t* ethernet_build_bytes_alloc(ethernet_t *ethernet, size_t* size, bool preamble);
void ethernet_from_bytes(ethernet_t *ethernet, uint8_t *buffer, size_t buffer_size, bool preamble);
void ethernet_print_dec(ethernet_t *ethernet);
void ethernet_print_hex(ethernet_t *ethernet);
void ethernet_print_bin(ethernet_t *ethernet);

uint32_t ethernet_compute_crc(ethernet_t *ethernet);

#endif
