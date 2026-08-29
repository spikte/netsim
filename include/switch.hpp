#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "interface.hpp"
#include "utils.hpp"
#include "device.hpp"
#include <unordered_map>
#include <unordered_set>
#include <cassert>

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
    bool is_vlan_allowed(uint16_t vlan);
    void add_vlan(uint16_t vlan);
    void rm_vlan(uint16_t vlan);
    void set_type(SwitchPortType type);
};
struct Switch : Device {
    std::vector<SwitchPort> ports;
    std::unordered_map<std::string, size_t> name_to_slot;
    std::unordered_map<MACAddress,size_t> mac_to_slot;
    bool on;
    MACTable mac_table;
    uint32_t bytes_per_tick;
    size_t process_interface_start;

    // Device related
    virtual size_t interface_count() const;
    virtual bool has_interface(size_t slot) const;
    virtual Interface& get_interface(size_t slot);
    virtual std::string interface_name(size_t slot) const;
    virtual std::optional<size_t> get_slot_by_name(const std::string& name) const;

    // Switch
    Switch(size_t nslot = 8);
    bool int_add(size_t slot, MACAddress address, uint32_t mtu = 1500, size_t buffer_size = 1024 * 1024);
    bool int_add(const std::string& name, MACAddress address, uint32_t mtu = 1500, size_t buffer_size = 1024 * 1024);
    bool int_del(size_t slot);
    bool int_del(const std::string& name);
    void int_turn_on(size_t slot);
    void int_turn_off(size_t slot);
    void int_turn_on(const std::string& name);
    void int_turn_off(const std::string& name);
    void int_set_type(size_t slot, SwitchPortType type);
    void int_set_type(const std::string& name, SwitchPortType type);
    void int_add_vlan(size_t slot, uint16_t vlan);
    void int_add_vlan(const std::string& name, uint16_t vlan);
    void int_rm_vlan(size_t slot, uint16_t vlan);
    void int_rm_vlan(const std::string& name, uint16_t vlan);
    bool link(size_t slot, Switch& neighbor, size_t neighbor_slot, Link& link);
    bool link(const std::string& name, Switch& neighbor, const std::string& name_neighbor, Link& l);
    bool send_packet(size_t slot, PacketRaw packet);
    bool is_send_valid(size_t from_slot, size_t to_slot, const ethernet_t& ethernet);
    PacketRaw process_packet(size_t from_slot, size_t to_slot, ethernet_t& ethernet);
    bool send_packet_from(size_t from_slot, size_t to_slot, PacketRaw packet);
    void broadcast(size_t from_slot, PacketRaw packet, uint16_t vlan);
    ssize_t process_packet_one(size_t slot);
    void update();
};

#endif
