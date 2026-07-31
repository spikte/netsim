#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "interface.hpp"
#include "utils.hpp"
extern "C" {
#include "slot_array.h"
}
#include <unordered_map>

struct MACTableEntry {
    size_t interface_id;
    uint32_t age;
};
struct MACTable {
    std::unordered_map<MACAddress,MACTableEntry> table;
    uint32_t age_limit;
    std::unordered_map<size_t,uint32_t> age_limit_int;

    MACTable();
    void add_entry(MACAddress address, size_t interface_id);
    void set_age_limit(uint32_t age_limit);
    void set_age_limit_int(size_t interface_id, uint32_t age_limit);
    void update();
};
struct Switch {
    SlotArray interfaces;
    bool on;
    BytesBudget bytes_budget;
    MACTable mac_table;

    Switch();
    void update();
};

#endif
