#include "../../include/sim/vlan_table.hpp"

void VLANTable::add_entry(std::uint16_t vlan, std::string name) {
    table[vlan] = VLANKey{.id     = vlan,
                          .name   = std::move(name),
                          .active = true};
}
void VLANTable::del_entry(std::uint16_t vlan) {
    table.erase(vlan);
}
void VLANTable::activate(std::uint16_t vlan) {
    auto it = table.find(vlan);
    if (it != table.end())
        it->second.active = true;
}
void VLANTable::deactivate(std::uint16_t vlan) {
    auto it = table.find(vlan);
    if (it != table.end())
        it->second.active = false;
}
