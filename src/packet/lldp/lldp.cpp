#include "../../../include/packet/lldp/lldp.hpp"
#include <stdexcept>

constexpr std::size_t LLDP_MAX_TLV_DATA_LEN = 511; // 9-bit length field
                                                   //

static void check_tlv_data_len(std::size_t len) {
    if (len > LLDP_MAX_TLV_DATA_LEN)
        throw std::invalid_argument("LLDP TLV length must be <= 511");
}

const char* LLDP::tlvtype_to_str(LLDP::TLVType tlv_type) {
    switch (tlv_type) {
        case LLDP::TLVType::EndOfLLDPDU:
            return "EndOfLLDPDU";
        case LLDP::TLVType::ChassisId:
            return "ChassisId";
        case LLDP::TLVType::PortId:
            return "PortId";
        case LLDP::TLVType::TimeToLive:
            return "TimeToLive";
        case LLDP::TLVType::PortDescription:
            return "PortDescription";
        case LLDP::TLVType::SystemName:
            return "SystemName";
        case LLDP::TLVType::SystemDescription:
            return "SystemDescription";
        case LLDP::TLVType::SystemCapabilities:
            return "SystemCapabilities";
        case LLDP::TLVType::ManagementAddress:
            return "ManagementAddress";
        case LLDP::TLVType::OrganizationalSpecific:
            return "OrganizationalSpecific";
    }

    return "Unknown";
}
const char* LLDP::chassisidsubtype_to_str(LLDP::ChassisIdSubtype tlv_type) {
    switch (tlv_type) {
        case LLDP::ChassisIdSubtype::Reserved:
            return "Reserved";
        case LLDP::ChassisIdSubtype::ChassisComponent:
            return "ChassisComponent";
        case LLDP::ChassisIdSubtype::InterfaceAlias:
            return "InterfaceAlias";
        case LLDP::ChassisIdSubtype::PortComponent:
            return "PortComponent";
        case LLDP::ChassisIdSubtype::MACAddress:
            return "MACAddress";
        case LLDP::ChassisIdSubtype::NetworkAddress:
            return "NetworkAddress";
        case LLDP::ChassisIdSubtype::InterfaceName:
            return "InterfaceName";
        case LLDP::ChassisIdSubtype::LocallyAssigned:
            return "LocallyAssigned";
    }

    return "Unknown";
}
const char* LLDP::portidsubtype_to_str(LLDP::PortIdSubtype tlv_type) {
    switch (tlv_type) {
        case LLDP::PortIdSubtype::Reserved:
            return "Reserved";
        case LLDP::PortIdSubtype::InterfaceAlias:
            return "InterfaceAlias";
        case LLDP::PortIdSubtype::PortComponent:
            return "PortComponent";
        case LLDP::PortIdSubtype::MACAddress:
            return "MACAddress";
        case LLDP::PortIdSubtype::NetworkAddress:
            return "NetworkAddress";
        case LLDP::PortIdSubtype::InterfaceName:
            return "InterfaceName";
        case LLDP::PortIdSubtype::AgentCircuitId:
            return "AgentCircuitId";
        case LLDP::PortIdSubtype::LocallyAssigned:
            return "LocallyAssigned";
    }

    return "Unknown";
}

/* TLV */
std::uint8_t TLV::get_type() const {
    return type;
}
void TLV::set_type(std::uint8_t type) {
    this->type = type;
}
std::uint16_t TLV::get_len() const {
    return static_cast<std::uint16_t>(data.size());
}
std::size_t TLV::size() const {
    return 2 + data.size();
}
std::size_t TLV::build_bytes(std::uint8_t* buffer) const {
    if (!buffer)
        return 0;
    check_tlv_data_len(data.size());
    const std::uint16_t type_len = static_cast<std::uint16_t>(
        (static_cast<std::uint16_t>(type) << 9) | (data.size() & 0x01FF));
    std::size_t offset = 0;
    unpack_u16_at(buffer, &offset, type_len);
    for (; offset - 2 < data.size(); offset++)
        buffer[offset] = data[offset - 2];
    return size();
}
std::vector<std::uint8_t> TLV::to_bytes() const {
    std::vector<std::uint8_t> buffer(size());

    build_bytes(buffer.data());

    return buffer;
}
bool TLV::from_bytes(const std::uint8_t* buffer, std::size_t size) {
    if (!buffer || size < 2)
        return false;
    const std::uint16_t type_len_ = (static_cast<std::uint16_t>(buffer[0]) << 8) |
                                    static_cast<std::uint16_t>(buffer[1]);
    const std::uint8_t type_      = static_cast<std::uint8_t>(type_len_ >> 9);
    const std::uint16_t len       = static_cast<std::uint16_t>(type_len_ & 0x01FF);
    if (size < 2 + len)
        return false;
    type = type_;
    data.assign(buffer + 2, buffer + 2 + len);

    return true;
}

/* LLDP */
void LLDP::add_tlv(LLDP::TLVType type, std::vector<std::uint8_t> data) {
    check_tlv_data_len(data.size());
    TLV tlv{};
    tlv.set_type(static_cast<std::uint8_t>(type));
    tlv.data = std::move(data);
    this->tlvs.push_back(std::move(tlv));
}
void LLDP::add_end_of_lldpdu() {
    add_tlv(LLDP::TLVType::EndOfLLDPDU, {});
}
void LLDP::add_chassis_id(ChassisIdSubtype chassis_id_subtype, std::vector<uint8_t> id) {
    id.insert(id.begin(), static_cast<std::uint8_t>(chassis_id_subtype));
    add_tlv(TLVType::ChassisId, std::move(id));
}
void LLDP::add_port_id(PortIdSubtype port_id_subtype, std::vector<uint8_t> id) {
    id.insert(id.begin(), static_cast<std::uint8_t>(port_id_subtype));
    add_tlv(TLVType::PortId, std::move(id));
}
void LLDP::add_ttl(std::uint16_t ttl) {
    std::vector<std::uint8_t> data(2);
    unpack_u16(data.data(), ttl);
    add_tlv(TLVType::TimeToLive, std::move(data));
}
void LLDP::add_port_description(const std::string& description) {
    std::vector<std::uint8_t> data(description.begin(), description.end());
    add_tlv(TLVType::PortDescription, std::move(data));
}
void LLDP::add_system_name(const std::string& name) {
    std::vector<std::uint8_t> data(name.begin(), name.end());
    add_tlv(TLVType::SystemName, std::move(data));
}
void LLDP::add_system_description(const std::string& description) {
    std::vector<std::uint8_t> data(description.begin(), description.end());
    add_tlv(TLVType::SystemDescription, std::move(data));
}
void LLDP::add_system_capabilities(std::uint16_t capabilities) {
    capabilities &= 0x7FF;
    std::vector<std::uint8_t> data(2);
    unpack_u16(data.data(), capabilities);
    add_tlv(TLVType::SystemCapabilities, std::move(data));
}
void LLDP::add_management_address(ManagementAddressSubtype management_address_subtype,
                                  std::vector<std::uint8_t> management_address,
                                  InterfaceNumberingSubtype interface_numbering_subtype,
                                  std::uint32_t interface_number,
                                  std::vector<std::uint8_t> oid) {
    if (management_address.size() + 1 > 0xFF)
        throw std::invalid_argument("Management address too long for its length field");
    if (oid.size() > 128)
        throw std::invalid_argument("OID size in Management Address TLV must be <= 128");

    std::vector<std::uint8_t> data;
    data.push_back(static_cast<std::uint8_t>(management_address.size() + 1));
    data.push_back(static_cast<std::uint8_t>(management_address_subtype));
    data.insert(data.end(), management_address.begin(), management_address.end());
    data.push_back(static_cast<std::uint8_t>(interface_numbering_subtype));
    std::vector<std::uint8_t> interface_number_unpacked(4);
    unpack_u32(interface_number_unpacked.data(), interface_number);
    data.insert(data.end(), interface_number_unpacked.begin(), interface_number_unpacked.end());
    data.push_back(static_cast<std::uint8_t>(oid.size()));
    if (!oid.empty())
        data.insert(data.end(), oid.begin(), oid.end());
    add_tlv(TLVType::ManagementAddress, std::move(data));
}
void LLDP::add_org_IEEE8021_port_vlan_id(std::uint16_t vlan_id) {
    std::vector<std::uint8_t> data(6);
    data[0] = 0x00;
    data[1] = 0x80;
    data[2] = 0xC2;
    data[3] = 1;
    unpack_u16(data.data() + 4, vlan_id);
    add_tlv(TLVType::OrganizationalSpecific, std::move(data));
}

std::size_t LLDP::size() const {
    std::size_t tlv_size;

    tlv_size = 0;
    for (auto const& tlv : tlvs)
        tlv_size += tlv.size();

    return tlv_size;
}
std::size_t LLDP::build_bytes(std::uint8_t* buffer) const {
    if (!buffer)
        return 0;
    std::size_t offset = 0;
    for (auto const& tlv : tlvs)
        offset += tlv.build_bytes(buffer + offset);
    return offset;
}
std::vector<std::uint8_t> LLDP::to_bytes() const {
    std::vector<std::uint8_t> buffer(size());

    build_bytes(buffer.data());

    return buffer;
}
bool LLDP::from_bytes(const std::uint8_t* buffer, std::size_t size) {
    if (!buffer)
        return false;

    tlvs.clear();
    std::size_t offset = 0;
    while (offset < size) {
        TLV tlv;
        if (!tlv.from_bytes(buffer + offset, size - offset))
            return false;
        offset += tlv.size();
        const auto type = static_cast<TLVType>(tlv.get_type());
        tlvs.push_back(std::move(tlv));
        if (type == TLVType::EndOfLLDPDU)
            return true;
    }

    return false;
}
