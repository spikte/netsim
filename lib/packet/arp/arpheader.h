#ifndef ARPHEADER_H
#define ARPHEADER_H

#include "../utils/base_convertion.h"
#include "../utils/numbers.h"
#include "../utils/print.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// From: https://www.iana.org/assignments/arp-parameters/arp-parameters.xhtml
enum ARPHardwareType {
    HTYPE_Reserved                   = 0,
    HTYPE_Ethernet_10Mb              = 1,
    HTYPE_Experimental_Ethernet_3Mb  = 2,
    HTYPE_Amateur_Radio_AX25         = 3,
    HTYPE_Proteon_ProNET_Token_Ring  = 4,
    HTYPE_Chaos                      = 5,
    HTYPE_IEEE_802_Networks          = 6,
    HTYPE_ARCNET                     = 7,
    HTYPE_Hyperchannel               = 8,
    HTYPE_Lanstar                    = 9,
    HTYPE_Autonet_Short_Address      = 10,
    HTYPE_LocalTalk                  = 11,
    HTYPE_LocalNet                   = 12,
    HTYPE_Ultra_Link                 = 13,
    HTYPE_SMDS                       = 14,
    HTYPE_Frame_Relay                = 15,
    HTYPE_ATM_16                     = 16,
    HTYPE_HDLC                       = 17,
    HTYPE_Fibre_Channel              = 18,
    HTYPE_ATM_19                     = 19,
    HTYPE_Serial_Line                = 20,
    HTYPE_ATM_21                     = 21,
    HTYPE_MIL_STD_188_220            = 22,
    HTYPE_Metricom                   = 23,
    HTYPE_IEEE_1394_1995             = 24,
    HTYPE_MAPOS                      = 25,
    HTYPE_Twinaxial                  = 26,
    HTYPE_EUI_64                     = 27,
    HTYPE_HIPARP                     = 28,
    HTYPE_IP_and_ARP_over_ISO_7816_3 = 29,
    HTYPE_ARPSec                     = 30,
    HTYPE_IPsec_Tunnel               = 31,
    HTYPE_InfiniBand                 = 32,
    HTYPE_TIA_102_Project_25_CAI     = 33,
    HTYPE_Wiegand_Interface          = 34,
    HTYPE_Pure_IP                    = 35,
    HTYPE_EXP1                       = 36,
    HTYPE_HFI                        = 37,
    HTYPE_Unified_Bus_UB             = 38,
    HTYPE_EXP2                       = 256,
    HTYPE_AEthernet                  = 257,
    HTYPE_Reserved_65535             = 65535
};
enum ARPOperation {
    ARPOP_Reserved               = 0,
    ARPOP_REQUEST                = 1,
    ARPOP_REPLY                  = 2,
    ARPOP_Request_Reverse        = 3,
    ARPOP_Reply_Reverse          = 4,
    ARPOP_DRARP_Request          = 5,
    ARPOP_DRARP_Reply            = 6,
    ARPOP_DRARP_Error            = 7,
    ARPOP_InARP_Request          = 8,
    ARPOP_InARP_Reply            = 9,
    ARPOP_ARP_NAK                = 10,
    ARPOP_MARS_Request           = 11,
    ARPOP_MARS_Multi             = 12,
    ARPOP_MARS_MServ             = 13,
    ARPOP_MARS_Join              = 14,
    ARPOP_MARS_Leave             = 15,
    ARPOP_MARS_NAK               = 16,
    ARPOP_MARS_Unserv            = 17,
    ARPOP_MARS_SJoin             = 18,
    ARPOP_MARS_SLeave            = 19,
    ARPOP_MARS_Grouplist_Request = 20,
    ARPOP_MARS_Grouplist_Reply   = 21,
    ARPOP_MARS_Redirect_Map      = 22,
    ARPOP_MAPOS_UNARP            = 23,
    ARPOP_EXP1                   = 24,
    ARPOP_EXP2                   = 25,
    ARPOP_Reserved_65535         = 65535
};
typedef struct {
    uint16_t htype;
    uint16_t ptype;
    uint8_t hlen;
    uint8_t plen;
    uint16_t oper;
    uint64_t sha;
    uint32_t spa;
    uint64_t tha;
    uint32_t tpa;
} arpheader_t;
void arpheader_init(arpheader_t* header);
void arpheader_set_htype(arpheader_t* header, enum ARPHardwareType htype);
void arpheader_set_ptype(arpheader_t* header, uint16_t ptype);
void arpheader_set_hlen(arpheader_t* header, uint8_t hlen);
void arpheader_set_plen(arpheader_t* header, uint8_t plen);
void arpheader_set_oper(arpheader_t* header, uint16_t oper);
void arpheader_set_sha(arpheader_t* header, uint64_t sha);
void arpheader_set_spa(arpheader_t* header, uint32_t spa);
void arpheader_set_tha(arpheader_t* header, uint64_t tha);
void arpheader_set_tpa(arpheader_t* header, uint32_t tha);

void arpheader_print_dec(arpheader_t* header);
void arpheader_print_hex(arpheader_t* header);
void arpheader_print_bin(arpheader_t* header);

#endif
