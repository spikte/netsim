#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "interface.hpp"
#include "utils.hpp"
#include <unordered_map>
#include <unordered_set>

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
enum class SwitchPortType {
    ACCESS,
    TRUNK
};
struct SwitchPort {
    Interface interface;
    std::string name;
    bool has_interface;
    SwitchPortType type;

    uint16_t access_vlan;
    std::unordered_set<uint16_t> allowed_vlans;
    uint16_t native_vlan;

    SwitchPort(std::string name = "");
    bool add_interface(MACAddress address, uint32_t mtu = 1500, size_t buffer_size = 1024 * 1024);
};
struct Switch {
    int nslot;
    std::vector<SwitchPort> ports;
    std::unordered_map<std::string, size_t> name_to_interface;
    bool on;
    MACTable mac_table;
    uint32_t bytes_per_tick;
    size_t process_interface_start;

    Switch(int nslot = 16);
    bool add_interface(size_t slot, MACAddress address, uint32_t mtu = 1500, size_t buffer_size = 1024 * 1024);
    bool del_interface(size_t slot);
    void turn_on_interface(size_t slot);
    void turn_off_interface(size_t slot);
    void turn_on_interface(std::string name);
    void turn_off_interface(std::string name);
    bool link(size_t slot, Switch& neighbor, size_t neighbor_slot, Link& link);
    bool send_packet(size_t slot, PacketRaw packet);
    void broadcast(PacketRaw packet, size_t from_slot);
    ssize_t process_packet_one(size_t slot);
    void update();
};

#endif
