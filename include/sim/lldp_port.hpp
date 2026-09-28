#ifndef LLDP_PORT_HPP
#define LLDP_PORT_HPP

#include "../packet/lldp/lldp.hpp"
#include "interface.hpp"
#include "type.hpp"

struct LLDPData {
};
struct LLDPNeighborData {
    LLDP::ChassisIdSubtype chassis_id_type;
    std::vector<std::uint8_t> chassis_id;
    LLDP::PortIdSubtype port_id_type;
    std::vector<std::uint8_t> port_id;
    std::uint16_t ttl;
    std::uint64_t last_received_tick;
};

struct LLDPPort {
    bool egress_enabled;
    bool ingress_enabled;
    std::uint64_t offset_tick;
    std::uint64_t last_send_tick;
    LLDP::PortIdSubtype port_id_type;
    std::vector<std::uint8_t> port_id;
    std::uint16_t ttl;
    LLDPData port_data;
    std::optional<LLDPNeighborData> neighbor_data;

    LLDPPort();
    void enable_egress();
    void enable_ingress();
    void disable_egress();
    void disable_ingress();
};

std::string chassis_id_to_str(LLDP::ChassisIdSubtype chassis_id_type, std::vector<uint8_t> id);
std::string port_id_to_str(LLDP::PortIdSubtype chassis_id_subtype, std::vector<uint8_t> id);

#endif
