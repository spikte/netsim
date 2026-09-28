#ifndef MAC_TABLE_HPP
#define MAC_TABLE_HPP

#include "interface.hpp"
#include <unordered_map>

struct MACTableKey {
    std::uint16_t vlan;
    MACAddress address;
    bool operator==(const MACTableKey& other) const;
};
template <>
struct std::hash<MACTableKey> {
    std::size_t operator()(const MACTableKey& k) const {
        return ((std::uint64_t) k.vlan << 48) | k.address;
    }
};
struct MACTableEntry {
    std::size_t interface_id;
    std::uint64_t aging_tick;
};
struct MACTable {
    std::unordered_map<MACTableKey, MACTableEntry> table;
    std::uint64_t age_limit;
    std::unordered_map<std::size_t, std::uint32_t> age_limit_int;

    MACTable();
    void add_entry(std::uint16_t vlan, MACAddress address, std::size_t interface_id);
    void del_entry(std::uint16_t vlan, MACAddress address);
    void set_age_limit(std::uint32_t age_limit);
    void set_age_limit_int(std::size_t interface_id, std::uint32_t age_limit);
    void update();
};

#endif
