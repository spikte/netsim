#ifndef SWITCHING_PROCESSOR_HPP
#define SWITCHING_PROCESSOR_HPP

#include "device.hpp"
#include "switch_port.hpp"
#include "lldp_processor.hpp"


struct SwitchingProcessor {
    static bool has_vlan(Device& device, std::uint16_t vlan);
    static bool port_set_type(Device& device, std::size_t port, SwitchPortType type);
    static bool port_set_type(Device& device, const std::string& name, SwitchPortType type);
    static bool port_add_vlan(Device& device, std::size_t port, std::uint16_t vlan);
    static bool port_add_vlan(Device& device, const std::string& name, std::uint16_t vlan);
    static bool port_rm_vlan(Device& device, std::size_t port, std::uint16_t vlan);
    static bool port_rm_vlan(Device& device, const std::string& name, std::uint16_t vlan);
    static std::vector<std::size_t> get_ports_in_vlan(Device& device, std::uint16_t vlan);
    static std::optional<std::uint16_t> ingress_vlan(Device& device, std::size_t port, const Ethernet& frame);
    static void learn_mac(Device& device, std::uint16_t vlan, MACAddress address, std::size_t port);
    static std::vector<std::size_t> vlan_ports(Device& device, std::uint16_t vlan, std::size_t except_port);
    static std::vector<std::size_t> forwarding_ports(Device& device, std::size_t ingress_port, std::uint16_t vlan, const MACAddress& destination);
    static void prepare_egress(Device& device, std::size_t ingress_port, std::size_t egress_port, std::uint16_t vlan, Ethernet& frame);
    static bool forward_frame(Device& device, std::size_t ingress_port, std::size_t egress_port, std::uint16_t vlan, Ethernet frame);
    static void process_frame_one(Device& device, std::size_t port, Ethernet& frame, std::uint16_t vlan);
};

#endif
