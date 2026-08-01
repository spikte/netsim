#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "interface.hpp"
#include "utils.hpp"
#include <unordered_map>

struct MACTableEntry {
    size_t interface_id;
    uint64_t aging_tick;
};
struct MACTable {
    std::unordered_map<MACAddress,MACTableEntry> table;
    uint64_t age_limit;
    std::unordered_map<size_t,uint32_t> age_limit_int;

    MACTable();
    void add_entry(MACAddress address, size_t interface_id);
    void set_age_limit(uint32_t age_limit);
    void set_age_limit_int(size_t interface_id, uint32_t age_limit);
    void update();
};
struct Switch {
    int nslot;
    std::vector<Interface> interfaces;
    std::vector<bool> slot_has_interface;
    std::unordered_map<std::string,size_t> name_to_interface;
    bool on;
    MACTable mac_table;
    uint32_t bytes_per_tick;
    size_t process_interface_start;

    Switch(int nslot = 16);
    bool add_interface(size_t slot, MACAddress address, std::string name, uint32_t mtu = 1500, size_t buffer_size = 1024 * 1024);
    bool del_interface(size_t slot);
    void turn_on_interface(size_t slot);
    void turn_off_interface(size_t slot);
    void turn_on_interface(std::string name);
    void turn_off_interface(std::string name);
    bool link(size_t slot, Switch& neighbor, size_t neighbor_slot, Link& link);
    bool send_packet(size_t slot, PacketRaw packet);
    void broadcast(PacketRaw packet, size_t from_slot);
    void update();
};

#endif
