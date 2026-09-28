#ifndef LLDP_HPP
#define LLDP_HPP

#include "../utils/numbers.h"
#include <cstdint>
#include <string>
#include <vector>

constexpr std::uint64_t LLDP_MULTICAST = 0x0180C200000E;

struct TLV {
    std::uint8_t type; // 7-bit TLV type
    std::vector<std::uint8_t> data;

    std::uint8_t get_type() const;
    void set_type(std::uint8_t type);
    std::uint16_t get_len() const;

    std::size_t size() const;
    std::size_t build_bytes(std::uint8_t* buffer) const;
    std::vector<std::uint8_t> to_bytes() const;
    bool from_bytes(const std::uint8_t* buffer, std::size_t size);
};
struct LLDP {
    enum class TLVType {
        EndOfLLDPDU            = 0,
        ChassisId              = 1,
        PortId                 = 2,
        TimeToLive             = 3,
        PortDescription        = 4,
        SystemName             = 5,
        SystemDescription      = 6,
        SystemCapabilities     = 7,
        ManagementAddress      = 8,
        OrganizationalSpecific = 127
    };
    static const char* tlvtype_to_str(LLDP::TLVType tlv_type);
    enum class ChassisIdSubtype {
        Reserved         = 0,
        ChassisComponent = 1,
        InterfaceAlias   = 2,
        PortComponent    = 3,
        MACAddress       = 4,
        NetworkAddress   = 5,
        InterfaceName    = 6,
        LocallyAssigned  = 7
    };
    static const char* chassisidsubtype_to_str(LLDP::ChassisIdSubtype tlv_type);
    enum class PortIdSubtype {
        Reserved        = 0,
        InterfaceAlias  = 1,
        PortComponent   = 2,
        MACAddress      = 3,
        NetworkAddress  = 4,
        InterfaceName   = 5,
        AgentCircuitId  = 6,
        LocallyAssigned = 7
    };
    static const char* portidsubtype_to_str(LLDP::PortIdSubtype tlv_type);
    // Not an `enum class` to be easily used as flags
    enum SystemCapability : std::uint16_t {
        Other             = 1u << 0,
        Repeater          = 1u << 1,
        Bridge            = 1u << 2,
        WlanAccessPoint   = 1u << 3,
        Router            = 1u << 4,
        Telephone         = 1u << 5,
        DocsisCableDevice = 1u << 6,
        StationOnly       = 1u << 7,
        CvlanComponent    = 1u << 8,
        SvlanComponent    = 1u << 9,
        TwoPortMacRelay   = 1u << 10
    };
    enum class ManagementAddressSubtype : uint8_t {
        Reserved          = 0,
        IPv4              = 1,
        IPv6              = 2,
        NSAP              = 3,
        HDLC              = 4,
        BBN1822           = 5,
        IEEE802           = 6,
        E163              = 7,
        E164              = 8,
        F69               = 9,
        X121              = 10,
        IPX               = 11,
        AppleTalk         = 12,
        DecnetIV          = 13,
        BanyanVines       = 14,
        E164WithNSAP      = 15,
        DNS               = 16,
        DistinguishedName = 17,
        ASNumber          = 18,
        X400              = 19,
        IEEE802_2         = 20,
    };
    enum class InterfaceNumberingSubtype : uint8_t {
        Unknown          = 1,
        IfIndex          = 2,
        SystemPortNumber = 3
    };
    std::vector<TLV> tlvs;
    void add_tlv(TLVType type, std::vector<uint8_t> data);
    void add_end_of_lldpdu();
    void add_chassis_id(ChassisIdSubtype chassis_id_subtype, std::vector<uint8_t> id);
    void add_port_id(PortIdSubtype chassis_id_subtype, std::vector<uint8_t> id);
    void add_ttl(std::uint16_t ttl);
    void add_port_description(const std::string& description);
    void add_system_name(const std::string& name);
    void add_system_description(const std::string& description);
    void add_system_capabilities(std::uint16_t capabilities);
    void add_management_address(ManagementAddressSubtype management_address_subtype, std::vector<std::uint8_t> management_address, InterfaceNumberingSubtype interface_numbering_subtype, std::uint32_t interface_number, std::vector<std::uint8_t> oid = {});

    void add_org_IEEE8021_port_vlan_id(std::uint16_t vlan_id);
    // TODO other organisational specific

    std::size_t size() const;
    std::size_t build_bytes(std::uint8_t* buffer) const;
    std::vector<std::uint8_t> to_bytes() const;
    bool from_bytes(const std::uint8_t* buffer, std::size_t size);
};

#endif
