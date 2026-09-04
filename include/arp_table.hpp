#ifndef ARP_TABLE_HPP
#define ARP_TABLE_HPP

#include "type.hpp"
#include <optional>
#include <unordered_map>
#include <string>
#include "settings.hpp"

struct ARPEntry {
    IPv4Address ipv4_address;
    MACAddress mac_address;
    std::string interface_name;
    uint64_t aging_tick;
};
struct ARPTable {
    std::unordered_map<IPv4Address, ARPEntry> table;
    uint64_t timeout;

    ARPTable();
    void add_entry(IPv4Address ipv4_address, MACAddress mac_address, std::string interface_name);
    void del_entry(IPv4Address ipv4_address);
    std::optional<ARPEntry> get(IPv4Address ipv4_address) const;
    std::optional<MACAddress> get_mac(IPv4Address ipv4_address) const;
    void update();
};


#endif
