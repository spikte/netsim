#ifndef SWITCH_PORT_HPP
#define SWITCH_PORT_HPP

#include "interface.hpp"
#include <unordered_set>

/* == SwitchPort == */
enum class SwitchPortType {
    ACCESS,
    TRUNK
};
struct SwitchPort {
    Interface* interface;
    SwitchPortType type;

    std::uint16_t access_vlan;
    std::unordered_set<std::uint16_t> allowed_vlans;
    std::uint16_t native_vlan;

    SwitchPort();
    bool is_vlan_allowed(std::uint16_t vlan) const;
    bool add_vlan(std::uint16_t vlan);
    bool rm_vlan(std::uint16_t vlan);
    bool set_type(SwitchPortType type);
    std::unordered_set<std::uint16_t> get_allowed_vlan() const;
};

#endif
