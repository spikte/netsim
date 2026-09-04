#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "device.hpp"
#include "interface.hpp"
#include "mac_table.hpp"
#include "utils.hpp"
#include <cassert>
#include <unordered_map>
#include <unordered_set>

/* == SwitchPort == */
enum class SwitchPortType {
    ACCESS,
    TRUNK
};
struct SwitchPort {
    Interface* interface;
    SwitchPortType type;

    uint16_t access_vlan;
    std::unordered_set<uint16_t> allowed_vlans;
    uint16_t native_vlan;

    SwitchPort();
    bool is_vlan_allowed(uint16_t vlan);
    void add_vlan(uint16_t vlan);
    void rm_vlan(uint16_t vlan);
    void set_type(SwitchPortType type);
};
/* == Switch == */
struct Switch : Device {
    std::vector<SwitchPort> ports;
    MACTable mac_table;

    // Switch
    Switch(size_t nport = 8);
    bool interface_add(size_t port, std::string name, MACAddress address, uint32_t mtu = 1500, size_t buffer_size = 1024 * 1024) override;
    bool interface_del(size_t port) override;
    void port_set_type(size_t port, SwitchPortType type);
    void port_set_type(const std::string& name, SwitchPortType type);
    void port_add_vlan(size_t port, uint16_t vlan);
    void port_add_vlan(const std::string& name, uint16_t vlan);
    void port_rm_vlan(size_t port, uint16_t vlan);
    void port_rm_vlan(const std::string& name, uint16_t vlan);
    bool is_send_valid(size_t from_port, size_t to_port, const ethernet_t& ethernet);
    PacketRaw process_packet(size_t from_port, size_t to_port, ethernet_t& ethernet);
    bool send_packet_from(size_t from_port, size_t to_port, PacketRaw packet);
    void broadcast(size_t from_port, PacketRaw packet, uint16_t vlan);
    void process_packet_arp(size_t port, PacketRaw& packet, ethernet_t* ethernet, uint16_t vlan);
    void process_packet(size_t port, PacketRaw& packet, ethernet_t* ethernet, uint16_t vlan);
    ssize_t process_packet_one(size_t port) override;
    void update() override;
};

#endif
