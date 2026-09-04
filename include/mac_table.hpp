#ifndef MAC_TABLE_HPP
#define MAC_TABLE_HPP

#include "interface.hpp"
#include <unordered_map>

struct MACTableKey {
    uint16_t vlan;
    MACAddress address;
    bool operator==(const MACTableKey& other) const;
};
template <>
struct std::hash<MACTableKey> {
    std::size_t operator()(const MACTableKey& k) const {
        return ((uint64_t) k.vlan << 48) | k.address;
    }
};
struct MACTableEntry {
    size_t interface_id;
    uint64_t aging_tick;
};
struct MACTable {
    std::unordered_map<MACTableKey, MACTableEntry> table;
    uint64_t age_limit;
    std::unordered_map<size_t, uint32_t> age_limit_int;

    MACTable();
    void add_entry(uint16_t vlan, MACAddress address, size_t interface_id);
    void set_age_limit(uint32_t age_limit);
    void set_age_limit_int(size_t interface_id, uint32_t age_limit);
    void update();
};

#endif
