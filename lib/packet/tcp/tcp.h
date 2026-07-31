#ifndef TCP_H
#define TCP_H

#include "tcpheader.h"
#include "tcpoption.h"
#include "../ipv4/ipv4.h"
#include <stdlib.h>
#include <stdint.h>


typedef struct {
   tcpheader_t header; 
   tcpoption_t option; 
   uint8_t* data; 
   size_t data_length;
} tcp_t;


uint16_t tcpheader_get_16bit(tcp_t* header, ipv4_t* ipv4, uint8_t index);
uint16_t tcpheader_compute_header_checksum(tcp_t* header, ipv4_t* ipv4);

#endif
