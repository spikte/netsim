#ifndef ETHERNETHEADER_H
#define ETHERNETHEADER_H

#include "../utils/base_convertion.h"
#include "../utils/numbers.h"
#include "../utils/print.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// I used the Wireshark source code for this file: https://raw.githubusercontent.com/wireshark/wireshark/refs/heads/master/epan/etypes.h

#define IEEE_802_3_MAX_LEN  1500
#define ETHERNET_II_MIN_LEN 1536

enum EtherType {
    ETHERTYPE_XNS_IDP               = 0x0600,
    ETHERTYPE_IP                    = 0x0800,
    ETHERTYPE_X25L3                 = 0x0805,
    ETHERTYPE_ARP                   = 0x0806,
    ETHERTYPE_WOL                   = 0x0842,
    ETHERTYPE_WMX_M2M               = 0x08F0,
    ETHERTYPE_BPQ                   = 0x08FF,
    ETHERTYPE_VINES_IP              = 0x0BAD,
    ETHERTYPE_VINES_ECHO            = 0x0BAF,
    ETHERTYPE_C15_HBEAT             = 0x0C15,
    ETHERTYPE_TRAIN                 = 0x1984,
    ETHERTYPE_CGMP                  = 0x2001,
    ETHERTYPE_GIGAMON               = 0x22E5,
    ETHERTYPE_MSRP                  = 0x22EA,
    ETHERTYPE_AVTP                  = 0x22F0,
    ETHERTYPE_ROHC                  = 0x22F1,
    ETHERTYPE_TRILL                 = 0x22F3,
    ETHERTYPE_L2ISIS                = 0x22F4,
    ETHERTYPE_CENTRINO_PROMISC      = 0x2452,
    ETHERTYPE_3C_NBP_DGRAM          = 0x3C07,
    ETHERTYPE_EPL_V1                = 0x3E3F,
    ETHERTYPE_TTL                   = 0x4040,
    ETHERTYPE_C15_CH                = 0x4742,
    ETHERTYPE_DEC                   = 0x6000,
    ETHERTYPE_DNA_DL                = 0x6001,
    ETHERTYPE_DNA_RC                = 0x6002,
    ETHERTYPE_DNA_RT                = 0x6003,
    ETHERTYPE_LAT                   = 0x6004,
    ETHERTYPE_DEC_DIAG              = 0x6005,
    ETHERTYPE_DEC_CUST              = 0x6006,
    ETHERTYPE_DEC_SCA               = 0x6007,
    ETHERTYPE_ETHBRIDGE             = 0x6558,
    ETHERTYPE_RAW_FR                = 0x6559,
    ETHERTYPE_REVARP                = 0x8035,
    ETHERTYPE_DEC_LB                = 0x8038,
    ETHERTYPE_DEC_LAST              = 0x8041,
    ETHERTYPE_ATALK                 = 0x809B,
    ETHERTYPE_SNA                   = 0x80D5,
    ETHERTYPE_DLR                   = 0x80E1,
    ETHERTYPE_AARP                  = 0x80F3,
    ETHERTYPE_VLAN                  = 0x8100,
    ETHERTYPE_SLPP                  = 0x8102,
    ETHERTYPE_VLACP                 = 0x8103,
    ETHERTYPE_OLDSLPP               = 0x8104,
    ETHERTYPE_NSRP                  = 0x8133,
    ETHERTYPE_IPX                   = 0x8137,
    ETHERTYPE_SNMP                  = 0x814C,
    ETHERTYPE_WCP                   = 0x80FF,
    ETHERTYPE_STP                   = 0x8181,
    ETHERTYPE_ISMP                  = 0x81FD,
    ETHERTYPE_ISMP_TBFLOOD          = 0x81FF,
    ETHERTYPE_QNX_QNET6             = 0x8204,
    ETHERTYPE_IPv6                  = 0x86DD,
    ETHERTYPE_WLCCP                 = 0x872D,
    ETHERTYPE_MINT                  = 0x8783,
    ETHERTYPE_MAC_CONTROL           = 0x8808,
    ETHERTYPE_SLOW_PROTOCOLS        = 0x8809,
    ETHERTYPE_PPP                   = 0x880B,
    ETHERTYPE_COBRANET              = 0x8819,
    ETHERTYPE_MPLS                  = 0x8847,
    ETHERTYPE_MPLS_MULTI            = 0x8848,
    ETHERTYPE_FOUNDRY               = 0x885A,
    ETHERTYPE_PPPOED                = 0x8863,
    ETHERTYPE_PPPOES                = 0x8864,
    ETHERTYPE_LINK_CTL              = 0x886C,
    ETHERTYPE_INTEL_ANS             = 0x886D,
    ETHERTYPE_MS_NLB_HEARTBEAT      = 0x886F,
    ETHERTYPE_JUMBO_LLC             = 0x8870,
    ETHERTYPE_BRCM_TYPE             = 0x8874,
    ETHERTYPE_HOMEPLUG              = 0x887B,
    ETHERTYPE_CDMA2000_A10_UBS      = 0x8881,
    ETHERTYPE_ATMOE                 = 0x8884,
    ETHERTYPE_EAPOL                 = 0x888E,
    ETHERTYPE_FORTINET_FGCP_HB      = 0x8890,
    ETHERTYPE_PROFINET              = 0x8892,
    ETHERTYPE_FORTINET_FGCP_SESSION = 0x8893,
    ETHERTYPE_REALTEK               = 0x8899,
    ETHERTYPE_HYPERSCSI             = 0x889A,
    ETHERTYPE_CSM_ENCAPS            = 0x889B,
    ETHERTYPE_TELKONET              = 0x88A1,
    ETHERTYPE_AOE                   = 0x88A2,
    ETHERTYPE_ECATF                 = 0x88A4,
    ETHERTYPE_IEEE_802_1AD          = 0x88A8,
    ETHERTYPE_IEEE_EXTREME_MESH     = 0x88A9,
    ETHERTYPE_EPL_V2                = 0x88AB,
    ETHERTYPE_XIMETA                = 0x88AD,
    ETHERTYPE_BRDWALK               = 0x88AE,
    ETHERTYPE_WAI                   = 0x88B4,
    ETHERTYPE_EXPERIMENTAL_ETH1     = 0x88B5,
    ETHERTYPE_EXPERIMENTAL_ETH2     = 0x88B6,
    ETHERTYPE_IEEE802_OUI_EXTENDED  = 0x88B7,
    ETHERTYPE_IEC61850_GOOSE        = 0x88B8,
    ETHERTYPE_IEC61850_GSE          = 0x88B9,
    ETHERTYPE_IEC61850_SV           = 0x88BA,
    ETHERTYPE_TIPC                  = 0x88CA,
    ETHERTYPE_RSN_PREAUTH           = 0x88C7,
    ETHERTYPE_LLDP                  = 0x88CC,
    ETHERTYPE_SERCOS                = 0x88CD,
    ETHERTYPE_3GPP2                 = 0x88D2,
    ETHERTYPE_CESOETH               = 0x88D8,
    ETHERTYPE_LLTD                  = 0x88D9,
    ETHERTYPE_WSMP                  = 0x88DC,
    ETHERTYPE_VMLAB                 = 0x88DE,
    ETHERTYPE_HOMEPLUG_AV           = 0x88E1,
    ETHERTYPE_MRP                   = 0x88E3,
    ETHERTYPE_MACSEC                = 0x88E5,
    ETHERTYPE_IEEE_802_1AH          = 0x88E7,
    ETHERTYPE_ELMI                  = 0x88EE,
    ETHERTYPE_MVRP                  = 0x88F5,
    ETHERTYPE_MMRP                  = 0x88F6,
    ETHERTYPE_PTP                   = 0x88F7,
    ETHERTYPE_NCSI                  = 0x88F8,
    ETHERTYPE_PRP                   = 0x88FB,
    ETHERTYPE_FLIP                  = 0x8901,
    ETHERTYPE_CFM                   = 0x8902,
    ETHERTYPE_DCE                   = 0x8903,
    ETHERTYPE_TTAG                  = 0x8905,
    ETHERTYPE_FCOE                  = 0x8906,
    ETHERTYPE_CMD                   = 0x8909,
    ETHERTYPE_IEEE80211_DATA_ENCAP  = 0x890d,
    ETHERTYPE_LINX                  = 0x8911,
    ETHERTYPE_FIP                   = 0x8914,
    ETHERTYPE_ROCE                  = 0x8915,
    ETHERTYPE_MIH                   = 0x8917,
    ETHERTYPE_TTE_PCF               = 0x891D,
    ETHERTYPE_VNTAG                 = 0x8926,
    ETHERTYPE_SEL_L2                = 0x892B,
    ETHERTYPE_BLUECOM               = 0x892D,
    ETHERTYPE_HSR                   = 0x892F,
    ETHERTYPE_IEEE_1905             = 0x893A,
    ETHERTYPE_IEEE_802_1BR          = 0x893F,
    ETHERTYPE_ECP                   = 0x8940,
    ETHERTYPE_ONOS                  = 0x8942,
    ETHERTYPE_GEONETWORKING         = 0x8947,
    ETHERTYPE_NSH                   = 0x894F,
    ETHERTYPE_PA_HBBACKUP           = 0x8988,
    ETHERTYPE_LOOP                  = 0x9000,
    ETHERTYPE_RTMAC                 = 0x9021,
    ETHERTYPE_RTCFG                 = 0x9022,
    ETHERTYPE_QINQ_OLD              = 0x9100,
    ETHERTYPE_EERO                  = 0x9104,
    ETHERTYPE_TECMP                 = 0x99FE,
    ETHERTYPE_6LOWPAN               = 0xA0ED,
    ETHERTYPE_ECPRI                 = 0xAEFE,
    ETHERTYPE_CABLELABS             = 0xB4E3,
    ETHERTYPE_XIP                   = 0xC0DE,
    ETHERTYPE_NWP                   = 0xC0DF,
    ETHERTYPE_LLT                   = 0xCAFE,
    ETHERTYPE_TDMOE                 = 0xD00D,
    ETHERTYPE_AVSP                  = 0xD28B,
    ETHERTYPE_EXEH                  = 0xE555,
    ETHERTYPE_ATMF                  = 0xFBAE,
    ETHERTYPE_ATRL                  = 0xFBAC,
    ETHERTYPE_FCFT                  = 0xFCFC,
    ETHERTYPE_ACIGLEAN              = 0xFFF2,
    ETHERTYPE_IEEE_802_1CB          = 0xF1C1
};
const char* ethheader_ethtype2str(enum EtherType eth_type);

typedef struct {
    uint64_t destination;
    uint64_t source;
    bool has_tag;
    uint32_t tag;
    uint16_t eth_type;
} ethernetheader_t;
void ethernetheader_init(ethernetheader_t* header);
size_t ethernetheader_size(const ethernetheader_t* header);
void ethernetheader_set_destination(ethernetheader_t* header, uint64_t destionation);
void ethernetheader_set_source(ethernetheader_t* header, uint64_t source);
void ethernetheader_set_has_tag(ethernetheader_t* header, bool has_tag);
void ethernetheader_set_tag(ethernetheader_t* header, uint32_t tag);
void ethernetheader_set_eth_type(ethernetheader_t* header, enum EtherType eth_type);

void ethernetheader_from_bytes(uint8_t* buffer, ethernetheader_t* header, bool preamble);
size_t ethernetheader_to_bytes(ethernetheader_t* header, uint8_t* buffer, bool preamble);

void ethernetheader_print_dec(ethernetheader_t* header);
void ethernetheader_print_hex(ethernetheader_t* header);
void ethernetheader_print_bin(ethernetheader_t* header);

typedef struct {
    uint32_t crc;
} ethernettail_t;
void ethernettail_init(ethernettail_t* header);

#endif
