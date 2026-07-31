#ifndef TCPHEADER_H
#define TCPHEADER_H

#include <stdint.h>


enum Control {
    CWR = 0b10000000,
    ECE = 0b01000000,
    URG = 0b00100000,
    ACK = 0b00010000,
    PSH = 0b00001000,
    RST = 0b00000100,
    SYN = 0b00000010,
    FIN = 0b00000001
};

typedef struct {
    uint16_t source;
    uint16_t destination;
    uint32_t sequence;
    uint32_t acknowledgment;
    uint8_t data_offset;
    uint8_t reserved;
    uint8_t controls;    
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent_pointer;
} tcpheader_t;

void tcpheader_set_source(tcpheader_t* tcpheader, uint16_t source);
void tcpheader_set_destination(tcpheader_t* tcpheader, uint16_t source);
void tcpheader_set_sequence(tcpheader_t* tcpheader, uint32_t sequence);
void tcpheader_set_acknowledgment(tcpheader_t* tcpheader, uint32_t acknowledgment);
void tcpheader_set_data_offset(tcpheader_t* tcpheader, uint32_t acknowledgment);
void tcpheader_set_reserved(tcpheader_t* tcpheader, uint8_t reserved);
void tcpheader_set_control(tcpheader_t* tcpheader, enum Control control);
void tcpheader_unset_control(tcpheader_t* tcpheader, enum Control control);
void tcpheader_set_controls(tcpheader_t* tcpheader, uint8_t controls);
void tcpheader_set_windows(tcpheader_t* tcpheader, uint16_t window);



#endif
