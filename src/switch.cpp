#include "../include/switch.hpp"

/* == SwitchPort == */
SwitchPort::SwitchPort() : interface(nullptr), type(SwitchPortType::ACCESS), access_vlan(1), native_vlan(1) {}
bool SwitchPort::is_vlan_allowed(uint16_t vlan) {
    vlan = vlan & 0x0FFF;
    if (type == SwitchPortType::ACCESS)
        return vlan == access_vlan;
    else
        return allowed_vlans.find(vlan) != allowed_vlans.end();
}
void SwitchPort::add_vlan(uint16_t vlan) {
    if (type == SwitchPortType::TRUNK)
        allowed_vlans.insert(vlan & 0x0FFF);
    else
        access_vlan = vlan;
}
void SwitchPort::rm_vlan(uint16_t vlan) {
    if (type == SwitchPortType::TRUNK) {
        if (auto iter = allowed_vlans.find(vlan); iter != allowed_vlans.end())
            allowed_vlans.erase(iter);
    } else {
        if (vlan == access_vlan)
            access_vlan = native_vlan;
    }
}
void SwitchPort::set_type(SwitchPortType type) {
    this->type = type;
}

/* == Switch == */
Switch::Switch(size_t nport) : Device(nport), mac_table({}) {
    ports.resize(nport);
}
bool Switch::interface_add(size_t port, std::string name, MACAddress address, uint32_t mtu, size_t buffer_size) {
    if (!Device::interface_add(port, name, address, mtu, buffer_size))
        return false;
    ports[port].interface = &interfaces[port];
    name_to_port[name]    = port;
    mac_to_port[address]  = port;
    return true;
}
bool Switch::interface_del(size_t port) {
    std::optional<std::string> name = interface_name(port);
    if (!name)
        return false;
    name_to_port.erase(*name);
    if (!Device::interface_del(port))
        return false;
    return true;
}
void Switch::port_set_type(size_t port, SwitchPortType type) {
    if (port >= ports.size())
        return;
    ports[port].set_type(type);
}
void Switch::port_set_type(const std::string& name, SwitchPortType type) {
    auto it = name_to_port.find(name);
    if (it == name_to_port.end())
        return;
    port_set_type(it->second, type);
}
void Switch::port_add_vlan(size_t port, uint16_t vlan) {
    if (port >= ports.size())
        return;
    ports[port].add_vlan(vlan);
}
void Switch::port_add_vlan(const std::string& name, uint16_t vlan) {
    auto it = name_to_port.find(name);
    if (it != name_to_port.end())
        port_add_vlan(it->second, vlan);
}
void Switch::port_rm_vlan(size_t port, uint16_t vlan) {
    if (port >= ports.size())
        return;
    ports[port].rm_vlan(vlan);
}
void Switch::port_rm_vlan(const std::string& name, uint16_t vlan) {
    auto it = name_to_port.find(name);
    if (it != name_to_port.end())
        port_rm_vlan(it->second, vlan);
}
bool Switch::is_send_valid(size_t from_port, size_t to_port, const ethernet_t& ethernet) {
    if (ports[from_port].type == SwitchPortType::ACCESS) {
        if (ethernet.header.has_tag)
            return false;
        return ports[to_port].is_vlan_allowed(ports[from_port].access_vlan);
    } else {
        uint16_t vlan;
        if (ethernet.header.has_tag)
            vlan = ethernet.header.tag & 0x0FFF;
        else
            vlan = ports[from_port].native_vlan;
        return ports[to_port].is_vlan_allowed(vlan) && ports[from_port].is_vlan_allowed(vlan);
    }
    return false;
}
PacketRaw Switch::process_packet(size_t from_port, size_t to_port, ethernet_t& ethernet) {
    PacketRaw packet;
    bool is_from_trunk;
    bool is_to_trunk;

    is_from_trunk = ports[from_port].type == SwitchPortType::TRUNK;
    is_to_trunk   = ports[to_port].type == SwitchPortType::TRUNK;
    if (!is_from_trunk && is_to_trunk)
        ethernetheader_set_vlan(&ethernet.header, ports[from_port].access_vlan);
    else if (is_from_trunk && !is_to_trunk)
        ethernetheader_set_has_tag(&ethernet.header, false);
    packet.resize(ethernet_size(&ethernet));
    ethernet_build_bytes(packet.data(), &ethernet, false);

    return packet;
}
bool Switch::send_packet_from(size_t from_port, size_t to_port, PacketRaw packet) {
    if (has_interface(from_port))
        return false;
    if (has_interface(to_port))
        return false;
    ethernet_t ethernet;
    PacketRaw packet_sent;

    ethernet_from_bytes(&ethernet, packet.data(), packet.size(), false);
    if (!is_send_valid(from_port, to_port, ethernet))
        return false;
    packet_sent = process_packet(from_port, to_port, ethernet);
    return send_packet(to_port, packet_sent);
}
void Switch::broadcast(size_t from_port, PacketRaw packet, uint16_t vlan) {
    for (size_t i = 0; i < ports.size(); i++) {
        if (!has_interface(i) || i == from_port)
            continue;
        send_packet_from(from_port, i, packet);
    }
}
void Switch::process_packet_arp(size_t port, PacketRaw& packet, ethernet_t* ethernet, uint16_t vlan) {
    broadcast(port, packet, vlan);
}
void Switch::process_packet(size_t port, PacketRaw& packet, ethernet_t* ethernet, uint16_t vlan) {
    switch(ethernet->header.eth_type) {
        case ETHERTYPE_ARP:
            process_packet_arp(port, packet, ethernet, vlan);
            break;
        default:
            break;
    }
}
ssize_t Switch::process_packet_one(size_t port) {
    if (!has_interface(port) || interfaces[port].recv_queued_bytes == 0)
        return -1;
    Interface* interface = ports[port].interface;
    PacketRaw& packet    = interface->recv_queue.front();
    size_t packet_size   = packet.size();
    uint16_t vlan;

    // We remove the size to recv_queued_bytes
    // now because then we move the data so we won't
    // be able to access the packet size
    interface->recv_queued_bytes -= packet.size();

    // Parse the packet ethernet header
    ethernet_t ethernet;
    ethernet_from_bytes(&ethernet, packet.data(), packet.size(), false);
    // Retrieve VLAN tag
    if (ports[port].type == SwitchPortType::ACCESS)
        vlan = ports[port].access_vlan;
    else {
        if (!ethernet.header.has_tag)
            vlan = ports[port].native_vlan;
        else
            vlan = ethernet.header.tag & (uint32_t) 0x0FFF;
    }

    // Learn the source address port
    mac_table.add_entry(vlan, ethernet.header.source, port);

    // If packet has arrived at destination process it
    if (mac_to_port.find(ethernet.header.destination) != mac_to_port.end()) {
        process_packet(port, packet, &ethernet, vlan);
        interface->recv_queue.pop_front();
        return packet_size;
    }

    // Else forward it
    MACTableKey mac_table_key = {vlan, ethernet.header.destination};
    auto it                   = mac_table.table.find(mac_table_key);
    if (it != mac_table.table.end())
        send_packet_from(port, it->second.interface_id, std::move(packet));
    else
        broadcast(port, std::move(packet), vlan);

    // Pop the packet from the interface sending it
    interface->recv_queue.pop_front();

    return packet_size;
}
void Switch::update() {
    if(!on)
        return;
    mac_table.update();
    Device::update();
}
