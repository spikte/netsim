#ifndef VLAN_TABLE_HPP
#define VLAN_TABLE_HPP

#include <cstdint>
#include <unordered_map>
#include <string>

struct VLANKey {
    std::uint16_t id;
    std::string name;
    bool active;
};
struct VLANTable {
    std::unordered_map <std::uint16_t, VLANKey> table;

    void add_entry(std::uint16_t vlan, std::string name);
    void del_entry(std::uint16_t vlan);
    void activate(std::uint16_t vlan);
    void deactivate(std::uint16_t vlan);
};

#endif
