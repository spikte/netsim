#include "../include/mac_table.hpp"

MACTable::MACTable() : table({}), age_limit(settings.mac_aging_default), age_limit_int({}) {}
bool MACTableKey::operator==(const MACTableKey& other) const {
    return vlan == other.vlan &&
           address == other.address;
}
void MACTable::add_entry(uint16_t vlan, MACAddress address, size_t interface_id) {
    MACTableKey key          = MACTableKey{vlan, address};
    auto it                  = table.find(key);
    auto age_limit_it        = age_limit_int.find(interface_id);
    uint64_t age_limit_entry = age_limit_it == age_limit_int.end() ? age_limit : age_limit_it->second;

    if (it != table.end()) {
        it->second.aging_tick   = global_tick + age_limit_entry;
        it->second.interface_id = interface_id;
    } else
        table[key] = MACTableEntry{interface_id, global_tick + age_limit_entry};
}
void MACTable::set_age_limit(uint32_t age_limit) {
    this->age_limit = age_limit;
}
void MACTable::set_age_limit_int(size_t interface_id, uint32_t age_limit) {
    this->age_limit_int[interface_id] = age_limit;
}
void MACTable::update() {
    // Delete old entry
    for (auto it = table.begin(); it != table.end();) {
        if (global_tick < it->second.aging_tick)
            it++;
        else
            it = table.erase(it);
    }
}
