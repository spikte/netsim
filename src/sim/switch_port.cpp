#include "../../include/sim/switch_port.hpp"

/* == SwitchPort == */
SwitchPort::SwitchPort() : interface(nullptr), type(SwitchPortType::ACCESS), access_vlan(1), native_vlan(1) {}
bool SwitchPort::is_vlan_allowed(std::uint16_t vlan) const {
    vlan = vlan & 0x0FFF;
    if (type == SwitchPortType::ACCESS)
        return vlan == access_vlan;
    else
        return allowed_vlans.find(vlan) != allowed_vlans.end();
}
bool SwitchPort::set_type(SwitchPortType type) {
    if (type == this->type)
        return false;
    this->type = type;
    return true;
}
bool SwitchPort::add_vlan(std::uint16_t vlan) {
    vlan &= 0x0FFF;
    if (type == SwitchPortType::TRUNK) {
        auto result = allowed_vlans.insert(vlan & 0x0FFF);
        return result.second;
    }
    if (access_vlan == vlan)
        return false;
    access_vlan = vlan;
    return true;
}
bool SwitchPort::rm_vlan(std::uint16_t vlan) {
    if (type == SwitchPortType::TRUNK) {
        if (auto iter = allowed_vlans.find(vlan); iter != allowed_vlans.end()) {
            allowed_vlans.erase(iter);
            return true;
        }
        return false;
    } else {
        if (vlan == access_vlan) {
            access_vlan = native_vlan;
            return true;
        }
        return false;
    }
}
std::unordered_set<std::uint16_t> SwitchPort::get_allowed_vlan() const {
    if (type == SwitchPortType::TRUNK) {
        return allowed_vlans;
    } else {
        return {access_vlan};
    }
}
