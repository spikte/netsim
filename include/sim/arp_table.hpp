#ifndef ARP_TABLE_HPP
#define ARP_TABLE_HPP

#include "type.hpp"
#include <optional>
#include <unordered_map>
#include <string>
#include "settings.hpp"

enum class ARPState {
    Pending,
    Resolved
};
struct ARPEntry {
    ARPState state;
    IPv4Address ipv4_address;
    MACAddress mac_address;
    std::string interface_name;
    std::uint64_t aging_tick;
};
struct ARPTable {
    std::unordered_map<IPv4Address, ARPEntry> table;
    std::uint64_t timeout;

    ARPTable();
    void add_entry(IPv4Address ipv4_address, MACAddress mac_address, std::string interface_name);
    void add_entry(IPv4Address ipv4_address);
    void del_entry(IPv4Address ipv4_address);
    std::optional<ARPEntry> get(IPv4Address ipv4_address) const;
    std::optional<MACAddress> get_mac(IPv4Address ipv4_address) const;
    void update();
};


#endif
