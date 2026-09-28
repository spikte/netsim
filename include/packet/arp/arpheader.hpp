#ifndef ARPHEADER_HPP
#define ARPHEADER_HPP

extern "C" {
#include "../utils/base_convertion.h"
#include "../utils/numbers.h"
}

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

// From: https://www.iana.org/assignments/arp-parameters/arp-parameters.xhtml
enum class ARPHardwareType : std::uint16_t {
    Reserved                   = 0,
    Ethernet_10Mb              = 1,
    Experimental_Ethernet_3Mb  = 2,
    Amateur_Radio_AX25         = 3,
    Proteon_ProNET_Token_Ring  = 4,
    Chaos                      = 5,
    IEEE_802_Networks          = 6,
    ARCNET                     = 7,
    Hyperchannel               = 8,
    Lanstar                    = 9,
    Autonet_Short_Address      = 10,
    LocalTalk                  = 11,
    LocalNet                   = 12,
    Ultra_Link                 = 13,
    SMDS                       = 14,
    Frame_Relay                = 15,
    ATM_16                     = 16,
    HDLC                       = 17,
    Fibre_Channel              = 18,
    ATM_19                     = 19,
    Serial_Line                = 20,
    ATM_21                     = 21,
    MIL_STD_188_220            = 22,
    Metricom                   = 23,
    IEEE_1394_1995             = 24,
    MAPOS                      = 25,
    Twinaxial                  = 26,
    EUI_64                     = 27,
    HIPARP                     = 28,
    IP_and_ARP_over_ISO_7816_3 = 29,
    ARPSec                     = 30,
    IPsec_Tunnel               = 31,
    InfiniBand                 = 32,
    TIA_102_Project_25_CAI     = 33,
    Wiegand_Interface          = 34,
    Pure_IP                    = 35,
    EXP1                       = 36,
    HFI                        = 37,
    Unified_Bus_UB             = 38,
    EXP2                       = 256,
    AEthernet                  = 257,
    Reserved_65535             = 65535
};

enum class ARPOperation : std::uint16_t {
    Reserved               = 0,
    REQUEST                = 1,
    REPLY                  = 2,
    Request_Reverse        = 3,
    Reply_Reverse          = 4,
    DRARP_Request          = 5,
    DRARP_Reply            = 6,
    DRARP_Error            = 7,
    InARP_Request          = 8,
    InARP_Reply            = 9,
    ARP_NAK                = 10,
    MARS_Request           = 11,
    MARS_Multi             = 12,
    MARS_MServ             = 13,
    MARS_Join              = 14,
    MARS_Leave             = 15,
    MARS_NAK               = 16,
    MARS_Unserv            = 17,
    MARS_SJoin             = 18,
    MARS_SLeave            = 19,
    MARS_Grouplist_Request = 20,
    MARS_Grouplist_Reply   = 21,
    MARS_Redirect_Map      = 22,
    MAPOS_UNARP            = 23,
    EXP1                   = 24,
    EXP2                   = 25,
    Reserved_65535         = 65535
};

struct ARPHeader {
    std::uint16_t htype;
    std::uint16_t ptype;
    std::uint8_t hlen;
    std::uint8_t plen;
    std::uint16_t oper;

    std::uint64_t sha;
    std::uint64_t spa;
    std::uint64_t tha;
    std::uint64_t tpa;

    void init();
    std::size_t size() const;

    void set_htype(ARPHardwareType htype);
    void set_ptype(std::uint16_t ptype);
    void set_hlen(std::uint8_t hlen);
    void set_plen(std::uint8_t plen);
    void set_oper(ARPOperation oper);
    void set_sha(std::uint64_t sha);
    void set_spa(std::uint32_t spa);
    void set_tha(std::uint64_t tha);
    void set_tpa(std::uint32_t tpa);

    void from_bytes(const std::uint8_t* buffer);
    std::size_t to_bytes(std::uint8_t* buffer) const;
    std::uint8_t* to_bytes_alloc(std::size_t* size = nullptr) const;

    static const char* htype_to_string(ARPHardwareType htype);
    static const char* oper_to_string(ARPOperation oper);
};

#endif
