#include "../../include/sim/arp_table.hpp"

ARPTable::ARPTable() : timeout(second_to_tick(60)) {};
void ARPTable::add_entry(IPv4Address ipv4_address, MACAddress mac_address, std::string interface_name) {
    table[ipv4_address] = ARPEntry(ARPState::Resolved, ipv4_address, mac_address, interface_name, global_tick + timeout);
}
void ARPTable::add_entry(IPv4Address ipv4_address) {
    table[ipv4_address] = ARPEntry(ARPState::Pending);
}
void ARPTable::del_entry(IPv4Address ipv4_address) {
    auto it = table.find(ipv4_address);
    if (it != table.end())
        table.erase(it);
}
std::optional<ARPEntry> ARPTable::get(IPv4Address ipv4_address) const {
    auto it = table.find(ipv4_address);
    if (it == table.end())
        return std::nullopt;
    return it->second;
}
std::optional<MACAddress> ARPTable::get_mac(IPv4Address ipv4_address) const {
    auto entry = get(ipv4_address);
    if (!entry)
        return std::nullopt;
    return entry->mac_address;
}
void ARPTable::update() {
    for (auto it = table.begin(); it != table.end();) {
        ARPEntry& entry = it->second;
        if (entry.aging_tick <= global_tick)
            it = table.erase(it);
        else
            it++;
    }
}
