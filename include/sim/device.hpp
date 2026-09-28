#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "../packet/packet.hpp"
#include "arp_packet_cache.hpp"
#include "arp_table.hpp"
#include "interface.hpp"
#include "lldp_port.hpp"
#include "mac_table.hpp"
#include "observer.hpp"
#include "packet_reassembly.hpp"
#include "routing_table.hpp"
#include "stp_port.hpp"
#include "switch_port.hpp"
#include "utils.hpp"
#include "vlan_table.hpp"
#include <string>
#include <unordered_set>

struct Device {
    std::string name;
    bool on;
    std::uint32_t bytes_per_tick;
    int64_t budget;
    std::size_t process_interface_start;
    std::vector<Interface> interfaces;
    std::vector<bool> has_interface_map;
    std::vector<Link*> links;
    std::unordered_map<std::string, std::size_t> name_to_port;
    std::unordered_map<MACAddress, std::size_t> mac_to_port;
    std::unordered_map<IPv4Address, std::size_t> ipv4_to_port;
    static MACAddress last_mac_address;
    Device(int nport = 8);
    virtual ~Device() = default;

    virtual void set_name(std::string name);
    virtual std::size_t port_count() const;
    virtual std::size_t interface_count() const;
    virtual bool is_valid_port(std::size_t port) const;
    virtual bool has_interface(std::size_t port) const;
    virtual bool has_interface_connected(std::size_t port) const;
    virtual bool is_interface_connectable_ingress(std::size_t port) const;
    virtual bool is_interface_connectable_egress(std::size_t port) const;
    virtual Interface* get_interface(std::size_t port);
    virtual Interface* get_interface(const std::string& name);
    virtual std::optional<std::size_t> get_interface_port(const std::string& name);
    virtual std::optional<std::size_t> get_interface_port(const MACAddress& address);
    virtual std::optional<std::size_t> get_interface_port(const IPv4Address& address);
    virtual std::optional<std::string> interface_name(std::size_t port) const;
    virtual bool interface_add(std::size_t port, std::string name, MACAddress address, std::uint32_t mtu = 1500, std::size_t buffer_size = 1024 * 1024);
    virtual bool interface_del(std::size_t port);
    virtual bool interface_del(const std::string interface_name);
    virtual bool interface_turn_on(std::size_t port);
    virtual bool interface_turn_off(std::size_t port);
    virtual bool interface_turn_on(const std::string& interface_name);
    virtual bool interface_turn_off(const std::string& interface_name);
    virtual void interface_set_mac_address(std::size_t port, const MACAddress& address);
    virtual void interface_set_ipv4(std::size_t port, const IPv4CIDR& ipv4_cidr);
    virtual void interface_set_name(std::size_t port, const std::string& name);
    virtual void update_interfaces_egress();
    virtual void update_interfaces_ingress();
    virtual bool init_update();
    virtual void update();

    // Ethernet
    virtual bool is_allowed_multicast_destination(std::uint64_t destination) const;
    virtual bool is_frame_destination(const Ethernet& frame) const;
    virtual bool is_frame_destination(std::size_t port, const Ethernet& frame) const;
    virtual bool schedule_ethernet(std::size_t port, MACAddress destination, std::optional<std::uint16_t> vlan, EthType eth_type, std::vector<std::uint8_t> data, std::uint64_t scheduled_tick);
    virtual bool send_ethernet(std::size_t port, MACAddress destination, std::optional<std::uint16_t> vlan, EthType eth_type, std::vector<std::uint8_t> data);
    virtual bool schedule_ethernet_broadcast(std::size_t port, std::optional<std::uint16_t> vlan, EthType eth_type, std::vector<std::uint8_t> data, std::uint64_t scheduled_tick);
    virtual bool send_ethernet_broadcast(std::size_t port, std::optional<std::uint16_t> vlan, EthType eth_type, std::vector<std::uint8_t> data);
    virtual std::optional<Ethernet> get_next_frame(std::size_t port);
    virtual int64_t process_frame(std::size_t port, Ethernet& frame) = 0;
    // Switching
    std::optional<std::vector<SwitchPort>> ports;
    std::optional<MACTable> mac_table;
    std::optional<VLANTable> vlan_table;
    // LLDP
    std::optional<LLDP::ChassisIdSubtype> chassis_id_type;
    std::optional<std::vector<std::uint8_t>> chassis_id;
    std::optional<std::vector<LLDPPort>> lldp_ports;
    virtual bool lldp_set_chassis_id(LLDP::ChassisIdSubtype chassis_id_type, std::vector<std::uint8_t> chassis_id);
    virtual bool lldp_set_chassis_id_mac(MACAddress mac_address);
    virtual bool lldp_set_chassis_id_mac(const std::string& mac_address_str);
    // ARP
    std::optional<ARPTable> arp_table;
    // Routing
    bool schedule_ipv4(std::size_t port, IPv4 packet, MACAddress destination, std::uint64_t scheduled_tick);
    bool send_ipv4(std::size_t port, IPv4 packet, MACAddress destination);
    std::optional<RoutingTable> routing_table;
    std::optional<ForwardingTable> forwarding_table;
    std::optional<PacketReassemblyTable> reassembly_table;
    std::optional<ARPPacketCache> arp_packet_cache;
    // ICMP
    Signal<IPv4&, ICMP&> on_icmp;
    // STP
    std::optional<STPVersion> stp_version;
    std::optional<std::unordered_map<std::uint16_t, STPDevice>> stp_devices;
};

#endif
