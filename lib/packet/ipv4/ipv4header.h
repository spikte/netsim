#ifndef IPV4HEADER_H
#define IPV4HEADER_H

#include "../utils/base_convertion.h"
#include "../utils/numbers.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum IPv4TOSPrecedence {
    IPV4TOS_NetworkControl      = 7,
    IPV4TOS_InternetworkControl = 6,
    IPV4TOS_CRITIC              = 5,
    IPV4TOS_FlashOverride       = 4,
    IPV4TOS_Flash               = 3,
    IPV4TOS_Immediate           = 2,
    IPV4TOS_Priority            = 1,
    IPV4TOS_Routine             = 0
};
const char* ipv4header_tosp2str(enum IPv4TOSPrecedence precedence);

enum IPv4Protocol {
    IPV4PROT_ICMP                         = 1,
    IPV4PROT_GatewayToGateway             = 3,
    IPV4PROT_CMCCGatewayMonitoringMessage = 4,
    IPV4PROT_ST                           = 5,
    IPV4PROT_TCP                          = 6,
    IPV4PROT_UCL                          = 7,
    IPV4PROT_Secure                       = 9,
    IPV4PROT_BBNRCCMonitoring             = 10,
    IPV4PROT_NVP                          = 11,
    IPV4PROT_PUP                          = 12,
    IPV4PROT_Pluribus                     = 13,
    IPV4PROT_Telenet                      = 14,
    IPV4PROT_XNET                         = 15,
    IPV4PROT_Chaos                        = 16,
    IPV4PROT_UserDatagram                 = 17,
    IPV4PROT_Multipv4lexing               = 18,
    IPV4PROT_DCN                          = 19,
    IPV4PROT_TACMonitoring                = 20,
    IPV4PROT_AnyLocalNetwork              = 63,
    IPV4PROT_SATNETAndBackroomEXPAK       = 64,
    IPV4PROT_MITSubnetSupport             = 65,
    IPV4PROT_SATNETMonitoring             = 69,
    IPV4PROT_InternetPacketCoreUtility    = 71,
    IPV4PROT_BackroomSATNETMonitoring     = 76,
    IPV4PROT_WIDEBANDMonitoring           = 78,
    IPV4PROT_WIDEBANDEXPAK                = 79
};
const char* ipv4header_protocol2str(enum IPv4Protocol protocol);

enum IPv4AddressFormat {
    A        = 0,
    B        = 2,
    C        = 6,
    Extended = 7
};
const char* ipv4header_format2str(enum IPv4AddressFormat format);

typedef struct {              // True size (in bits)   Checksum 16 bits groups
    uint8_t version_length;   // 8                     0
    uint8_t type_of_service;  // 8                     0
    uint16_t total_length;    // 16                    1
    uint16_t identification;  // 16                    2
    uint16_t fragment_offset; // 16                    3
    uint8_t time_to_live;     // 8                     4
    uint8_t protocol;         // 8                     4
    uint16_t checksum;        // 16                    NONE
    uint32_t source;          // 32                    6-7
    uint32_t destination;     // 32                    8-9
} ipv4header_t;

/* Init */
void ipv4header_init(ipv4header_t* header);

/* Version */
void ipv4header_set_version(ipv4header_t* header, uint8_t version);

/* Length */
void ipv4header_set_length(ipv4header_t* header, uint8_t length);

/* Type of service */
void ipv4header_set_type_of_service_precedence(ipv4header_t* header, enum IPv4TOSPrecedence type_of_service);
void ipv4header_set_type_of_service_delay(ipv4header_t* header, uint8_t delay);
void ipv4header_set_type_of_service_throughput(ipv4header_t* header, uint8_t throughput);
void ipv4header_set_type_of_service_relibility(ipv4header_t* header, uint8_t relibility);

/* Identification */
void ipv4header_set_identification(ipv4header_t* header, uint16_t identification);

/* Flags */
void ipv4header_set_flags_DF(ipv4header_t* header, uint8_t DF);
void ipv4header_set_flags_MF(ipv4header_t* header, uint8_t MF);

/* Fragment Offset */
void ipv4header_set_fragment_offset(ipv4header_t* header, uint16_t fragment_offset);

/* Time to Live */
void ipv4header_set_time_to_live(ipv4header_t* header, uint8_t time_to_live);

/* Protocol */
void ipv4header_set_protocol(ipv4header_t* header, enum IPv4Protocol protocol);

/* Address */
void ipv4header_set_address_classful_format(uint32_t* address, enum IPv4AddressFormat format);
void ipv4header_set_address_classful_network(uint32_t* address, enum IPv4AddressFormat format, uint32_t network);
void ipv4header_set_address_classful_host(uint32_t* address, enum IPv4AddressFormat format, uint32_t host);
void ipv4header_set_address_network(uint32_t* address, uint32_t network, uint8_t prefix);
void ipv4header_set_address_host(uint32_t* address, uint32_t host, uint8_t prefix);

/* Source Address */
void ipv4header_set_source_classful(ipv4header_t* header, enum IPv4AddressFormat format, uint32_t network, uint32_t host);
void ipv4header_set_source(ipv4header_t* header, uint32_t network, uint32_t host, uint8_t prefix);
/* Destination Address */
void ipv4header_set_destination_classful(ipv4header_t* header, enum IPv4AddressFormat format, uint32_t network, uint32_t host);
void ipv4header_set_destination(ipv4header_t* header, uint32_t network, uint32_t host, uint8_t prefix);

void ipv4header_from_bytes(uint8_t* buffer, ipv4header_t* header);
void ipv4header_to_bytes(const ipv4header_t* header, uint8_t* buffer);
void ipv4header_print_dec(ipv4header_t* ipv4);
void ipv4header_print_hex(ipv4header_t* ipv4);
void ipv4header_print_bin(ipv4header_t* ipv4);

#endif
