#include "../../include/sim/lldp_port.hpp"

LLDPPort::LLDPPort() : egress_enabled(false),
                       ingress_enabled(false),
                       offset_tick(second_to_tick(0.2)),
                       last_send_tick(0),
                       port_id_type(LLDP::PortIdSubtype::InterfaceName),
                       port_id({}),
                       ttl(second_to_tick(0.5)),
                       neighbor_data(std::nullopt) {}
void LLDPPort::enable_egress() {
    egress_enabled = true;
    last_send_tick = global_tick;
}
void LLDPPort::enable_ingress() {
    ingress_enabled = true;
}
void LLDPPort::disable_egress() {
    egress_enabled = false;
}
void LLDPPort::disable_ingress() {
    ingress_enabled = false;
}

std::string chassis_id_to_str(LLDP::ChassisIdSubtype chassis_id_type, std::vector<uint8_t> id) {
    std::string chassis_id_str;

    switch (static_cast<LLDP::ChassisIdSubtype>(chassis_id_type)) {
        case LLDP::ChassisIdSubtype::MACAddress: {
            MACAddress address = pack_u48(id.data());
            chassis_id_str     = mac2str(address);
            break;
        }
        case LLDP::ChassisIdSubtype::NetworkAddress: {
            IPv4Address address = pack_u32(id.data());
            chassis_id_str      = ipv42str(address);
            break;
        }
        case LLDP::ChassisIdSubtype::InterfaceName: {
            chassis_id_str = std::string(id.begin(), id.end());
            break;
        }
        default:
            chassis_id_str = bytes2str(id);
            break;
    }

    return chassis_id_str;
}
std::string port_id_to_str(LLDP::PortIdSubtype port_id_type, std::vector<uint8_t> id) {
    std::string port_id_str;

    switch (static_cast<LLDP::PortIdSubtype>(port_id_type)) {
        case LLDP::PortIdSubtype::MACAddress: {
            MACAddress address = pack_u48(id.data());
            port_id_str        = mac2str(address);
            break;
        }
        case LLDP::PortIdSubtype::NetworkAddress: {
            IPv4Address address = pack_u32(id.data());
            port_id_str         = ipv42str(address);
            break;
        }
        case LLDP::PortIdSubtype::InterfaceAlias:
        case LLDP::PortIdSubtype::InterfaceName: {
            port_id_str = std::string(id.begin(), id.end());
            break;
        }
        default:
            port_id_str = bytes2str(id);
            break;
    }

    return port_id_str;
}
