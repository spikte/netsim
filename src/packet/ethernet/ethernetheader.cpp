#include "../../../include/packet/ethernet/ethernetheader.hpp"

const char* ethheader_ethtype2str(EthType eth_type) {
    switch (eth_type) {
        case EthType::IP:
            return "IPv4";

        case EthType::IPv6:
            return "IPv6";

        case EthType::VLAN:
            return "802.1Q Virtual LAN";

        case EthType::SLPP:
            return "Simple Loop Protection Protocol";

        case EthType::VLACP:
            return "Virtual LACP";

        case EthType::ARP:
            return "ARP";

        case EthType::REVARP:
            return "RARP";

        case EthType::STP:
            return "Spanning Tree Protocol";

        case EthType::PPPOED:
            return "PPPoE Discovery";

        case EthType::PPPOES:
            return "PPPoE Session";

        case EthType::MPLS:
            return "MPLS label switched packet";

        case EthType::MPLS_MULTI:
            return "MPLS multicast label switched packet";

        case EthType::LLDP:
            return "802.1 Link Layer Discovery Protocol (LLDP)";

        case EthType::EAPOL:
            return "802.1X Authentication";

        case EthType::MACSEC:
            return "802.1AE (MACsec)";

        case EthType::FCOE:
            return "Fibre Channel over Ethernet";

        case EthType::ROCE:
            return "RDMA over Converged Ethernet";

        case EthType::PTP:
            return "PTPv2 over Ethernet (IEEE1588)";

        case EthType::PRP:
            return "Parallel Redundancy Protocol";

        case EthType::HSR:
            return "High-availability Seamless Redundancy";

        case EthType::ECPRI:
            return "eCPRI";

        case EthType::_6LOWPAN:
            return "6LoWPAN";

        case EthType::IEEE_802_1AD:
            return "802.1ad Provider Bridge (Q-in-Q)";

        case EthType::QINQ_OLD:
            return "QinQ: old non-standard 802.1ad";

        default:
            return "No string rep";
    }
}

std::uint8_t VlanTag::get_pcp() const {
    return static_cast<std::uint8_t>((tci >> 13) & 0x07);
}
std::uint8_t VlanTag::get_dei() const {
    return static_cast<std::uint8_t>((tci >> 12) & 0x01);
}
std::uint16_t VlanTag::get_vid() const {
    return tci & 0x0FFF;
}
void VlanTag::set_pcp(std::uint8_t pcp) {
    tci &= ~(0x07u << 13);
    tci |= static_cast<std::uint16_t>((pcp & 0x07) << 13);
}
void VlanTag::set_dei(std::uint8_t dei) {
    tci &= ~(0x01u << 12);
    tci |= static_cast<std::uint16_t>((dei & 0x01) << 12);
}
void VlanTag::set_vid(std::uint16_t vid) {
    tci &= ~0x0FFFu;
    tci |= vid & 0x0FFF;
}

EthernetHeader::EthernetHeader() {
    init();
}
void EthernetHeader::init() {
    destination = 0;
    source      = 0;
    tags.clear();
    eth_type = 0;
}
std::size_t EthernetHeader::size() const {
    return 14 + tags.size() * 4;
}
void EthernetHeader::set_destination(std::uint64_t value) {
    destination = value;
}
void EthernetHeader::set_source(std::uint64_t value) {
    source = value;
}
void EthernetHeader::set_eth_type(EthType value) {
    eth_type = static_cast<std::uint16_t>(value);
}

/* VLAN manipulation. */
/* - Push front */
bool EthernetHeader::push_front_vlan(std::uint16_t tpid, std::uint16_t tci) {
    if (!is_valid_pid(tpid))
        return false;

    tags.insert(tags.begin(), VlanTag{tpid, tci});

    return true;
}
bool EthernetHeader::push_front_8021q(std::uint16_t tci) {
    return push_front_vlan(static_cast<std::uint16_t>(EthType::VLAN), tci);
}
bool EthernetHeader::push_front_8021ad(std::uint16_t tci) {
    return push_front_vlan(static_cast<std::uint16_t>(EthType::IEEE_802_1AD), tci);
}
bool EthernetHeader::push_front_qinq(std::uint16_t tci) {
    return push_front_vlan(static_cast<std::uint16_t>(EthType::QINQ_OLD), tci);
}
bool EthernetHeader::push_front_vlan_id(std::uint16_t vlan_id) {
    if (vlan_id > 4095)
        return false;

    return push_front_vlan(static_cast<std::uint16_t>(EthType::VLAN), vlan_id);
}
bool EthernetHeader::push_front_vlan_8021ad_id(std::uint16_t vlan_id) {
    if (vlan_id > 4095)
        return false;

    return push_front_vlan(static_cast<std::uint16_t>(EthType::IEEE_802_1AD), vlan_id);
}
bool EthernetHeader::push_front_vlan_qinq_id(std::uint16_t vlan_id) {
    if (vlan_id > 4095)
        return false;

    return push_front_vlan(static_cast<std::uint16_t>(EthType::QINQ_OLD), vlan_id);
}
/* - Push back */
bool EthernetHeader::push_back_vlan(std::uint16_t tpid, std::uint16_t tci) {
    if (!is_valid_pid(tpid))
        return false;

    tags.push_back({tpid, tci});

    return true;
}
bool EthernetHeader::push_back_8021q(std::uint16_t tci) {
    return push_back_vlan(static_cast<std::uint16_t>(EthType::VLAN), tci);
}
bool EthernetHeader::push_back_8021ad(std::uint16_t tci) {
    return push_back_vlan(static_cast<std::uint16_t>(EthType::IEEE_802_1AD), tci);
}
bool EthernetHeader::push_back_qinq(std::uint16_t tci) {
    return push_back_vlan(static_cast<std::uint16_t>(EthType::QINQ_OLD), tci);
}
bool EthernetHeader::push_back_vlan_id(std::uint16_t vlan_id) {
    if (vlan_id > 4095)
        return false;

    return push_back_vlan(static_cast<std::uint16_t>(EthType::VLAN), vlan_id);
}
bool EthernetHeader::push_back_vlan_8021ad_id(std::uint16_t vlan_id) {
    if (vlan_id > 4095)
        return false;

    return push_back_vlan(static_cast<std::uint16_t>(EthType::IEEE_802_1AD), vlan_id);
}
bool EthernetHeader::push_back_vlan_qinq_id(std::uint16_t vlan_id) {
    if (vlan_id > 4095)
        return false;

    return push_back_vlan(static_cast<std::uint16_t>(EthType::QINQ_OLD), vlan_id);
}
/* - Set */
bool EthernetHeader::set_vlan(std::size_t depth, std::uint16_t tpid, std::uint16_t tci) {
    if (!is_valid_pid(tpid) || depth >= tags.size())
        return false;

    tags[depth].tpid = tpid;
    tags[depth].tci  = tci;

    return true;
}
bool EthernetHeader::set_8021q(std::size_t depth, std::uint16_t tci) {
    return set_vlan(depth, static_cast<std::uint16_t>(EthType::VLAN), tci);
}
bool EthernetHeader::set_8021ad(std::size_t depth, std::uint16_t tci) {
    return set_vlan(depth, static_cast<std::uint16_t>(EthType::IEEE_802_1AD), tci);
}
bool EthernetHeader::set_qinq(std::size_t depth, std::uint16_t tci) {
    return set_vlan(depth, static_cast<std::uint16_t>(EthType::QINQ_OLD), tci);
}
bool EthernetHeader::set_vlan_id(std::size_t depth, std::uint16_t vlan_id) {
    if (vlan_id > 4095)
        return false;

    return set_vlan(depth, static_cast<std::uint16_t>(EthType::VLAN), vlan_id);
}
bool EthernetHeader::set_vlan_8021ad_id(std::size_t depth, std::uint16_t vlan_id) {
    if (vlan_id > 4095)
        return false;

    return set_vlan(depth, static_cast<std::uint16_t>(EthType::IEEE_802_1AD), vlan_id);
}
bool EthernetHeader::set_vlan_qinq_id(std::size_t depth, std::uint16_t vlan_id) {
    if (vlan_id > 4095)
        return false;

    return set_vlan(depth, static_cast<std::uint16_t>(EthType::QINQ_OLD), vlan_id);
}
/* - Other */
bool EthernetHeader::get_vlan(std::size_t depth, VlanTag& tag) const {
    if (depth >= tags.size())
        return false;

    tag = tags[depth];

    return true;
}
bool EthernetHeader::pop_vlan(VlanTag* tag) {
    if (tags.empty())
        return false;

    if (tag)
        *tag = tags.front();

    tags.erase(tags.begin());

    return true;
}
void EthernetHeader::clear_vlans() {
    tags.clear();
}

bool EthernetHeader::is_valid_pid(std::uint16_t tpid) {
    switch (tpid) {
        case static_cast<std::uint16_t>(EthType::VLAN):
        case static_cast<std::uint16_t>(EthType::IEEE_802_1AD):
        case static_cast<std::uint16_t>(EthType::QINQ_OLD):
            return true;
        default:
            return false;
    }
}
bool EthernetHeader::is_u64_mac_address(std::uint64_t address) {
    return (address >> 48) == 0;
}
bool EthernetHeader::is_mac_address_multicast(std::uint64_t address) {
    return (address & 0x010000000000ULL) != 0;
}

std::size_t EthernetHeader::build_bytes(std::uint8_t* buffer, bool preamble) const {
    if (!buffer)
        return 0;

    std::size_t offset = 0;
    if (preamble) {
        for (std::size_t i = 0; i < 7; ++i)
            buffer[offset++] = ETHERHEADER_PREAMBLE;
        buffer[offset++] = ETHERHEADER_SFD;
    }

    unpack_u48_at(buffer, &offset, destination);
    unpack_u48_at(buffer, &offset, source);
    for (const auto& tag : tags) {
        unpack_u16_at(buffer, &offset, tag.tpid);
        unpack_u16_at(buffer, &offset, tag.tci);
    }
    unpack_u16_at(buffer, &offset, eth_type);

    return offset;
}
std::vector<std::uint8_t> EthernetHeader::to_bytes(bool preamble) const {
    std::vector<std::uint8_t> buffer(size());

    build_bytes(buffer.data(), preamble);

    return buffer;
}
bool EthernetHeader::from_bytes(const std::uint8_t* buffer, std::size_t buffer_size, bool preamble) {
    if (!buffer)
        return false;

    const std::size_t base_offset = preamble ? 8 : 0;
    if (preamble) {
        if (buffer_size < 22)
            return false;

        for (std::size_t i = 0; i < 7; ++i) {
            if (buffer[i] != ETHERHEADER_PREAMBLE)
                return false;
        }

        if (buffer[7] != ETHERHEADER_SFD)
            return false;
    } else {
        if (buffer_size < 14)
            return false;
    }

    init();
    std::size_t offset = base_offset;
    destination        = pack_u48_at(buffer, &offset);
    source             = pack_u48_at(buffer, &offset);
    if (is_mac_address_multicast(source)) {
        init();
        return false;
    }
    std::uint16_t type = pack_u16_at(buffer, &offset);

    while (is_valid_pid(type)) {
        if (offset + 2 > buffer_size) {
            init();
            return false;
        }
        const std::uint16_t tci = pack_u16_at(buffer, &offset);
        if (!push_back_vlan(type, tci)) {
            init();
            return false;
        }
        if (offset + 2 > buffer_size) {
            init();
            return false;
        }
        type = pack_u16_at(buffer, &offset);
    }

    eth_type = type;

    return true;
}

EthernetTail::EthernetTail() {
    init();
}
void EthernetTail::init() {
    crc = 0;
}
