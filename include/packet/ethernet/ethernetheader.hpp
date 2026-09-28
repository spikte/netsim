#ifndef ETHERNETHEADER_HPP
#define ETHERNETHEADER_HPP

extern "C" {
#include "../utils/numbers.h"
}

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <vector>

constexpr std::uint8_t ETHERHEADER_PREAMBLE  = 0x55;
constexpr std::uint8_t ETHERHEADER_SFD       = 0xD5;
constexpr std::uint16_t IEEE_802_3_MAX_LEN   = 1500;
constexpr std::uint16_t ETHERNET_II_MIN_LEN  = 1536;
constexpr std::uint64_t BROADCAST_MAC = 0xFFFFFFFFFFFF;

enum class EthType : std::uint16_t {
    XNS_IDP               = 0x0600,
    IP                    = 0x0800,
    X25L3                 = 0x0805,
    ARP                   = 0x0806,
    WOL                   = 0x0842,
    WMX_M2M               = 0x08F0,
    BPQ                   = 0x08FF,
    VINES_IP              = 0x0BAD,
    VINES_ECHO            = 0x0BAF,
    C15_HBEAT             = 0x0C15,
    TRAIN                 = 0x1984,
    CGMP                  = 0x2001,
    GIGAMON               = 0x22E5,
    MSRP                  = 0x22EA,
    AVTP                  = 0x22F0,
    ROHC                  = 0x22F1,
    TRILL                 = 0x22F3,
    L2ISIS                = 0x22F4,
    CENTRINO_PROMISC      = 0x2452,
    _3C_NBP_DGRAM         = 0x3C07,
    EPL_V1                = 0x3E3F,
    TTL                   = 0x4040,
    C15_CH                = 0x4742,
    DEC                   = 0x6000,
    DNA_DL                = 0x6001,
    DNA_RC                = 0x6002,
    DNA_RT                = 0x6003,
    LAT                   = 0x6004,
    DEC_DIAG              = 0x6005,
    DEC_CUST              = 0x6006,
    DEC_SCA               = 0x6007,
    ETHBRIDGE             = 0x6558,
    RAW_FR                = 0x6559,
    REVARP                = 0x8035,
    DEC_LB                = 0x8038,
    DEC_LAST              = 0x8041,
    ATALK                 = 0x809B,
    SNA                   = 0x80D5,
    DLR                   = 0x80E1,
    AARP                  = 0x80F3,
    VLAN                  = 0x8100,
    SLPP                  = 0x8102,
    VLACP                 = 0x8103,
    OLDSLPP               = 0x8104,
    NSRP                  = 0x8133,
    IPX                   = 0x8137,
    SNMP                  = 0x814C,
    WCP                   = 0x80FF,
    STP                   = 0x8181,
    ISMP                  = 0x81FD,
    ISMP_TBFLOOD          = 0x81FF,
    QNX_QNET6             = 0x8204,
    IPv6                  = 0x86DD,
    WLCCP                 = 0x872D,
    MINT                  = 0x8783,
    MAC_CONTROL           = 0x8808,
    SLOW_PROTOCOLS        = 0x8809,
    PPP                   = 0x880B,
    COBRANET              = 0x8819,
    MPLS                  = 0x8847,
    MPLS_MULTI            = 0x8848,
    FOUNDRY               = 0x885A,
    PPPOED                = 0x8863,
    PPPOES                = 0x8864,
    LINK_CTL              = 0x886C,
    INTEL_ANS             = 0x886D,
    MS_NLB_HEARTBEAT      = 0x886F,
    JUMBO_LLC             = 0x8870,
    BRCM_TYPE             = 0x8874,
    HOMEPLUG              = 0x887B,
    CDMA2000_A10_UBS      = 0x8881,
    ATMOE                 = 0x8884,
    EAPOL                 = 0x888E,
    FORTINET_FGCP_HB      = 0x8890,
    PROFINET              = 0x8892,
    FORTINET_FGCP_SESSION = 0x8893,
    REALTEK               = 0x8899,
    HYPERSCSI             = 0x889A,
    CSM_ENCAPS            = 0x889B,
    TELKONET              = 0x88A1,
    AOE                   = 0x88A2,
    ECATF                 = 0x88A4,
    IEEE_802_1AD          = 0x88A8,
    IEEE_EXTREME_MESH     = 0x88A9,
    EPL_V2                = 0x88AB,
    XIMETA                = 0x88AD,
    BRDWALK               = 0x88AE,
    WAI                   = 0x88B4,
    EXPERIMENTAL_ETH1     = 0x88B5,
    EXPERIMENTAL_ETH2     = 0x88B6,
    IEEE802_OUI_EXTENDED  = 0x88B7,
    IEC61850_GOOSE        = 0x88B8,
    IEC61850_GSE          = 0x88B9,
    IEC61850_SV           = 0x88BA,
    TIPC                  = 0x88CA,
    RSN_PREAUTH           = 0x88C7,
    LLDP                  = 0x88CC,
    SERCOS                = 0x88CD,
    _3GPP2                = 0x88D2,
    CESOETH               = 0x88D8,
    LLTD                  = 0x88D9,
    WSMP                  = 0x88DC,
    VMLAB                 = 0x88DE,
    HOMEPLUG_AV           = 0x88E1,
    MRP                   = 0x88E3,
    MACSEC                = 0x88E5,
    IEEE_802_1AH          = 0x88E7,
    ELMI                  = 0x88EE,
    MVRP                  = 0x88F5,
    MMRP                  = 0x88F6,
    PTP                   = 0x88F7,
    NCSI                  = 0x88F8,
    PRP                   = 0x88FB,
    FLIP                  = 0x8901,
    CFM                   = 0x8902,
    DCE                   = 0x8903,
    TTAG                  = 0x8905,
    FCOE                  = 0x8906,
    CMD                   = 0x8909,
    IEEE80211_DATA_ENCAP  = 0x890D,
    LINX                  = 0x8911,
    FIP                   = 0x8914,
    ROCE                  = 0x8915,
    MIH                   = 0x8917,
    TTE_PCF               = 0x891D,
    VNTAG                 = 0x8926,
    SEL_L2                = 0x892B,
    BLUECOM               = 0x892D,
    HSR                   = 0x892F,
    IEEE_1905             = 0x893A,
    IEEE_802_1BR          = 0x893F,
    ECP                   = 0x8940,
    ONOS                  = 0x8942,
    GEONETWORKING         = 0x8947,
    NSH                   = 0x894F,
    PA_HBBACKUP           = 0x8988,
    LOOP                  = 0x9000,
    RTMAC                 = 0x9021,
    RTCFG                 = 0x9022,
    QINQ_OLD              = 0x9100,
    EERO                  = 0x9104,
    TECMP                 = 0x99FE,
    _6LOWPAN              = 0xA0ED,
    ECPRI                 = 0xAEFE,
    CABLELABS             = 0xB4E3,
    XIP                   = 0xC0DE,
    NWP                   = 0xC0DF,
    LLT                   = 0xCAFE,
    TDMOE                 = 0xD00D,
    AVSP                  = 0xD28B,
    EXEH                  = 0xE555,
    ATMF                  = 0xFBAE,
    ATRL                  = 0xFBAC,
    FCFT                  = 0xFCFC,
    ACIGLEAN              = 0xFFF2,
    IEEE_802_1CB          = 0xF1C1
};
const char* ethheader_ethtype2str(EthType eth_type);

struct VlanTag {
    std::uint16_t tpid{};
    std::uint16_t tci{};

    std::uint8_t get_pcp() const;
    std::uint8_t get_dei() const;
    std::uint16_t get_vid() const;
    void set_pcp(std::uint8_t pcp);
    void set_dei(std::uint8_t dei);
    void set_vid(std::uint16_t vid);
};

class EthernetHeader {
  public:
    std::uint64_t destination{};
    std::uint64_t source{};
    std::vector<VlanTag> tags;
    std::uint16_t eth_type{};

    EthernetHeader();
    void init();
    std::size_t size() const;

    void set_destination(std::uint64_t destination);
    void set_source(std::uint64_t source);
    void set_eth_type(EthType eth_type);

    bool push_front_vlan(std::uint16_t tpid, std::uint16_t tci);
    bool push_front_8021q(std::uint16_t tci);
    bool push_front_8021ad(std::uint16_t tci);
    bool push_front_qinq(std::uint16_t tci);
    bool push_front_vlan_id(std::uint16_t vlan_id);
    bool push_front_vlan_8021ad_id(std::uint16_t vlan_id);
    bool push_front_vlan_qinq_id(std::uint16_t vlan_id);
    bool push_back_vlan(std::uint16_t tpid, std::uint16_t tci);
    bool push_back_8021q(std::uint16_t tci);
    bool push_back_8021ad(std::uint16_t tci);
    bool push_back_qinq(std::uint16_t tci);
    bool push_back_vlan_id(std::uint16_t vlan_id);
    bool push_back_vlan_8021ad_id(std::uint16_t vlan_id);
    bool push_back_vlan_qinq_id(std::uint16_t vlan_id);

    bool set_vlan(std::size_t depth, std::uint16_t tpid, std::uint16_t tci);
    bool set_8021q(std::size_t depth, std::uint16_t tci);
    bool set_8021ad(std::size_t depth, std::uint16_t tci);
    bool set_qinq(std::size_t depth, std::uint16_t tci);

    bool set_vlan_id(std::size_t depth, std::uint16_t vlan_id);
    bool set_vlan_8021ad_id(std::size_t depth, std::uint16_t vlan_id);
    bool set_vlan_qinq_id(std::size_t depth, std::uint16_t vlan_id);

    bool get_vlan(std::size_t depth, VlanTag& tag) const;
    bool pop_vlan(VlanTag* tag = nullptr);
    void clear_vlans();

    static bool is_valid_pid(std::uint16_t tpid);
    static bool is_u64_mac_address(std::uint64_t address);
    static bool is_mac_address_multicast(std::uint64_t address);
    static bool is_valid(const std::uint8_t* buffer, std::size_t size, bool preamble);

    std::size_t build_bytes(std::uint8_t* buffer, bool preamble = false) const;
    std::vector<std::uint8_t> to_bytes(bool preamble = false) const;
    bool from_bytes(const std::uint8_t* buffer, std::size_t size, bool preamble);
};

struct EthernetTail {
    std::uint32_t crc{};

    EthernetTail();
    void init();
};

#endif
