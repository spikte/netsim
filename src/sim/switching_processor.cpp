#include "../../include/sim/switching_processor.hpp"

// This on i have to admit i asked an AI
// but good to know it exists
static std::string format_vlan_id(std::uint16_t vlan) {
    std::ostringstream ss;
    ss << "VLAN"
       << std::setw(4)
       << std::setfill('0')
       << vlan;

    return ss.str();
}

bool SwitchingProcessor::has_vlan(Device& device, std::uint16_t vlan) {
    return device.vlan_table->table.find(vlan) != device.vlan_table->table.end();
}
bool SwitchingProcessor::port_set_type(Device& device, std::size_t port, SwitchPortType type) {
    if (port >= device.ports->size())
        return false;
    (*device.ports)[port].set_type(type);
    return true;
}
bool SwitchingProcessor::port_set_type(Device& device, const std::string& name, SwitchPortType type) {
    auto it = device.name_to_port.find(name);
    if (it != device.name_to_port.end())
        return port_set_type(device, it->second, type);
    return false;
}
bool SwitchingProcessor::port_add_vlan(Device& device, std::size_t port, std::uint16_t vlan) {
    if (port >= device.ports->size())
        return false;
    auto it = device.vlan_table->table.find(vlan);
    if (it == device.vlan_table->table.end())
        device.vlan_table->add_entry(vlan, format_vlan_id(vlan));
    return (*device.ports)[port].add_vlan(vlan);
}
bool SwitchingProcessor::port_add_vlan(Device& device, const std::string& name, std::uint16_t vlan) {
    auto it = device.name_to_port.find(name);
    if (it != device.name_to_port.end())
        return port_add_vlan(device, it->second, vlan);
    return false;
}
bool SwitchingProcessor::port_rm_vlan(Device& device, std::size_t port, std::uint16_t vlan) {
    if (port >= device.ports->size())
        return false;
    return (*device.ports)[port].rm_vlan(vlan);
}
bool SwitchingProcessor::port_rm_vlan(Device& device, const std::string& name, std::uint16_t vlan) {
    auto it = device.name_to_port.find(name);
    if (it != device.name_to_port.end())
        return port_rm_vlan(device, it->second, vlan);
    return false;
}
std::vector<std::size_t> SwitchingProcessor::get_ports_in_vlan(Device& device, std::uint16_t vlan) {
    std::vector<std::size_t> ports_in_vlan;

    for (std::size_t port = 0; port < device.ports->size(); port++) {
        if (!device.has_interface(port))
            continue;
        SwitchPort& switch_port = (*device.ports)[port];
        if (switch_port.is_vlan_allowed(vlan))
            ports_in_vlan.push_back(port);
    }

    return ports_in_vlan;
}
std::optional<std::uint16_t> SwitchingProcessor::ingress_vlan(Device& device, std::size_t port, const Ethernet& frame) {
    const SwitchPort& p = (*device.ports)[port];
    if (p.type == SwitchPortType::ACCESS) {
        if (frame.header.tags.size() != 0)
            return std::nullopt;
        return p.access_vlan;
    }
    // TRUNK
    std::uint16_t vlan;
    if (frame.header.tags.size() == 0)
        vlan = p.native_vlan;
    else
        vlan = frame.header.tags[0].get_vid();
    if (!p.is_vlan_allowed(vlan))
        return std::nullopt;
    return vlan;
}
void SwitchingProcessor::learn_mac(Device& device, std::uint16_t vlan, MACAddress address, std::size_t port) {
    device.mac_table->add_entry(vlan, address, port);
}
std::vector<std::size_t> SwitchingProcessor::vlan_ports(Device& device, std::uint16_t vlan, std::size_t except_port) {
    std::vector<std::size_t> result;

    for (std::size_t port = 0; port < device.ports->size(); ++port) {
        if (!device.has_interface(port))
            continue;
        if (port == except_port)
            continue;
        if ((*device.ports)[port].is_vlan_allowed(vlan))
            result.push_back(port);
    }

    return result;
}
std::vector<std::size_t> SwitchingProcessor::forwarding_ports(Device& device, std::size_t ingress_port, std::uint16_t vlan, const MACAddress& destination) {
    MACTableKey key{vlan, destination};
    auto it = device.mac_table->table.find(key);
    if (it != device.mac_table->table.end())
        return {it->second.interface_id};
    return vlan_ports(device, vlan, ingress_port);
}
void SwitchingProcessor::prepare_egress(Device& device, std::size_t ingress_port, std::size_t egress_port, std::uint16_t vlan, Ethernet& frame) {
    SwitchPort& port_ingress = (*device.ports)[ingress_port];
    SwitchPort& port_egress  = (*device.ports)[egress_port];
    bool is_ingress_trunk    = port_ingress.type == SwitchPortType::TRUNK;
    bool is_egress_trunk     = port_egress.type == SwitchPortType::TRUNK;

    if (!is_ingress_trunk && is_egress_trunk && vlan != port_egress.native_vlan)
        frame.header.push_front_vlan_id((*device.ports)[ingress_port].access_vlan);
    else if (is_ingress_trunk && !is_egress_trunk && frame.header.tags.size() > 0)
        frame.header.pop_vlan();
    frame.tail.crc = frame.compute_crc();
}
bool SwitchingProcessor::forward_frame(Device& device, std::size_t ingress_port, std::size_t egress_port, std::uint16_t vlan, Ethernet frame) {
    if(!device.has_interface_connected(egress_port))
        return false;
    if (!(*device.ports)[egress_port].is_vlan_allowed(vlan))
        return false;
    prepare_egress(device, ingress_port, egress_port, vlan, frame);
    DataRaw data;
    data.resize(frame.size());
    frame.build_bytes(data.data(), false);
    device.interfaces[egress_port].schedule(std::move(data), global_tick);
    return true;
}
void SwitchingProcessor::process_frame_one(Device& device, std::size_t port, Ethernet& frame, std::uint16_t vlan) {
    switch (static_cast<EthType>(frame.header.eth_type)) {
        case EthType::LLDP:
            LLDPProcessor::process_frame_lldp(device, port, frame);
            break;
        default:
            break;
    }
}
