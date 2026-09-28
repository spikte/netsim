#include "../../include/sim/device.hpp"
#include "../../include/sim/lldp_processor.hpp"

MACAddress Device::last_mac_address = *str2mac_hex("02:00:00:00:00:01");
Device::Device(int nport) : name("Device"), on(true), bytes_per_tick(1024 * 1024), process_interface_start(0) {
    interfaces.resize(nport);
    links.resize(nport);
    for (std::size_t port = 0; port < interfaces.size(); port++) {
        interfaces[port].device      = this;
        interfaces[port].device_port = port;
    }
    has_interface_map.resize(nport, false);
}
void Device::set_name(std::string name) {
    this->name = name;
}
std::size_t Device::port_count() const {
    return interfaces.size();
}
std::size_t Device::interface_count() const {
    std::size_t count;

    count = 0;
    for (std::size_t i = 0; i < has_interface_map.size(); i++)
        count += has_interface_map[i];

    return count;
}
bool Device::is_valid_port(std::size_t port) const {
    return port < interfaces.size();
}
bool Device::has_interface(std::size_t port) const {
    return is_valid_port(port) && has_interface_map[port];
}
bool Device::has_interface_connected(std::size_t port) const {
    return has_interface(port) && (interfaces[port].ingress || interfaces[port].egress);
}
bool Device::is_interface_connectable_ingress(std::size_t port) const {
    return has_interface(port) && !interfaces[port].ingress;
}
bool Device::is_interface_connectable_egress(std::size_t port) const {
    return has_interface(port) && !interfaces[port].egress;
}
Interface* Device::get_interface(std::size_t port) {
    if (has_interface(port))
        return &interfaces[port];
    return nullptr;
}
Interface* Device::get_interface(const std::string& name) {
    auto it = name_to_port.find(name);
    if (it != name_to_port.end())
        return get_interface(it->second);
    return nullptr;
}
std::optional<std::size_t> Device::get_interface_port(const std::string& name) {
    auto it = name_to_port.find(name);
    if (it != name_to_port.end())
        return it->second;
    return std::nullopt;
}
std::optional<std::size_t> Device::get_interface_port(const MACAddress& address) {
    auto it = mac_to_port.find(address);
    if (it != mac_to_port.end())
        return it->second;
    return std::nullopt;
}
std::optional<std::size_t> Device::get_interface_port(const IPv4Address& address) {
    auto it = ipv4_to_port.find(address);
    if (it != ipv4_to_port.end())
        return it->second;
    return std::nullopt;
}
std::optional<std::string> Device::interface_name(std::size_t port) const {
    if (has_interface(port))
        return interfaces[port].name;
    return std::nullopt;
}
bool Device::interface_add(std::size_t port, std::string name, MACAddress mac_address, std::uint32_t mtu, std::size_t buffer_size) {
    if (!is_valid_port(port) || has_interface_map[port])
        return false;
    interfaces[port].name              = name;
    interfaces[port].mac_address       = mac_address;
    interfaces[port].ipv4_cidr         = IPv4CIDR(0, (IPv4Address) 0);
    interfaces[port].mtu               = mtu;
    interfaces[port].buffer_size       = buffer_size;
    interfaces[port].ingress           = nullptr;
    interfaces[port].egress            = nullptr;
    interfaces[port].send_queued_bytes = 0;
    interfaces[port].recv_queued_bytes = 0;
    interfaces[port].send_queue.clear();
    interfaces[port].recv_queue.clear();
    name_to_port[name]       = port;
    mac_to_port[mac_address] = port;
    has_interface_map[port]  = true;
    if (ports.has_value())
        (*ports)[port].interface = &interfaces[port];
    if (lldp_ports.has_value()) {
        (*lldp_ports)[port].port_id_type = LLDP::PortIdSubtype::InterfaceName;
        (*lldp_ports)[port].port_id      = std::vector<std::uint8_t>(name.begin(), name.end());
    }
    interfaces[port].type            = InterfaceType::Ethernet;
    interfaces[port].ethernet_config = EthernetConfig{
        .physical_port_type = PhysicalPortType::RJ45,
        .phy                = EthernetPhy::_1000BASE_T,
    };
    interfaces[port].bits_per_second = Gbs(1);

    return true;
}
bool Device::interface_del(std::size_t port) {
    if (!has_interface(port))
        return false;
    if (routing_table.has_value())
        routing_table->del_entry_connected(interfaces[port].ipv4_cidr, interfaces[port].name);

    Interface& interface = interfaces[port];
    if (interface.ingress)
        interface.ingress->disconnect_dst();
    if (interface.egress)
        interface.egress->disconnect_src();

    name_to_port.erase(interface.name);
    mac_to_port.erase(interface.mac_address);

    if (interface.ipv4_cidr.address != 0)
        ipv4_to_port.erase(interface.ipv4_cidr.address);

    has_interface_map[port] = false;

    if (ports.has_value()) {
        if (mac_table.has_value()) {
            for (std::uint16_t vlan_id : (*ports)[port].get_allowed_vlan())
                mac_table->del_entry(interface.mac_address, vlan_id);
        }
        (*ports)[port] = SwitchPort{};
    }

    if (lldp_ports.has_value())
        (*lldp_ports)[port] = LLDPPort();

    interfaces[port] = Interface();

    return true;
}
bool Device::interface_del(const std::string interface_name) {
    auto it = name_to_port.find(interface_name);
    if (it != name_to_port.end())
        return interface_del(it->second);
    return false;
}
bool Device::interface_turn_on(std::size_t port) {
    if (!has_interface(port) || interfaces[port].on)
        return false;
    interfaces[port].on = true;

    return true;
}
bool Device::interface_turn_off(std::size_t port) {
    if (!has_interface(port) || !interfaces[port].on)
        return false;
    interfaces[port].on = false;

    return true;
}
bool Device::interface_turn_on(const std::string& interface_name) {
    auto it = name_to_port.find(interface_name);
    if (it != name_to_port.end())
        return interface_turn_on(it->second);
    return false;
}
bool Device::interface_turn_off(const std::string& interface_name) {
    auto it = name_to_port.find(interface_name);
    if (it != name_to_port.end())
        return interface_turn_off(it->second);
    return false;
}
void Device::interface_set_mac_address(std::size_t port, const MACAddress& address) {
    if (!has_interface(port))
        return;
    mac_to_port.erase(interfaces[port].mac_address);
    interfaces[port].mac_address = address;
    mac_to_port[address]         = port;
}
void Device::interface_set_ipv4(std::size_t port, const IPv4CIDR& ipv4_cidr) {
    if (!has_interface(port))
        return;
    if (routing_table.has_value()) {
        routing_table->del_entry_connected(interfaces[port].ipv4_cidr, interfaces[port].name);
        routing_table->add_entry(ipv4_cidr, 0, interfaces[port].name, 0, RoutingTableProtocol::Connected, 0);
        IPv4CIDR ipv4_cidr_full = ipv4_cidr;
        ipv4_cidr_full.set_prefix(32);
        routing_table->add_entry(ipv4_cidr_full, 0, interfaces[port].name, 0, RoutingTableProtocol::Local, 0);
    }
    ipv4_to_port.erase(interfaces[port].ipv4_cidr.address);
    interfaces[port].ipv4_cidr      = ipv4_cidr;
    ipv4_to_port[ipv4_cidr.address] = port;
    if (forwarding_table.has_value())
        forwarding_table->build(*routing_table);
}
void Device::interface_set_name(std::size_t port, const std::string& name) {
    if (!has_interface(port))
        return;
    name_to_port.erase(interfaces[port].name);
    interfaces[port].name = name;
    name_to_port[name]    = port;
}
void Device::update_interfaces_egress() {
    const bool has_lldp_port = lldp_ports.has_value();
    for (std::size_t i = 0; i < port_count(); i++) {
        if (has_interface(i)) {
            if (has_lldp_port)
                LLDPProcessor::update_egress(*this, i);
            interfaces[i].update();
        }
    }
}
void Device::update_interfaces_ingress() {
    bool progress = true;
    while (budget > 0 && progress) {
        progress = false;
        for (std::size_t i = 0; i < port_count(); i++) {
            std::size_t port = (process_interface_start + i) % port_count();
            if (!has_interface(port) || interfaces[port].recv_queued_bytes == 0)
                continue;
            auto frame_opt = get_next_frame(port);
            if (!frame_opt.has_value()) {
                interfaces[port].recv_queue.pop_front();
                continue;
            }
            Ethernet frame            = std::move(*frame_opt);
            int64_t process_data_size = process_frame(port, frame);
            interfaces[port].recv_queue.pop_front();
            if (process_data_size < 0)
                continue;
            progress = true;
            budget -= process_data_size;
        }
    }
    process_interface_start = (process_interface_start + 1) % port_count();
}
bool Device::init_update() {
    if (!on)
        return false;
    budget = bytes_per_tick;

    return true;
}
void Device::update() {
    if (!init_update())
        return;
    update_interfaces_ingress();
}

// Ethernet
bool Device::is_allowed_multicast_destination(std::uint64_t destination) const {
    if (destination == BROADCAST_MAC)
        return true;
    const bool multicast = (destination & (1ULL << 40)) != 0;
    if (!multicast)
        return false;
    // Protocol specific multicast addresses
    switch (destination) {
        case LLDP_MULTICAST:
            return true;
        default:
            return false;
    }
}
bool Device::is_frame_destination(const Ethernet& frame) const {
    const auto destination = frame.header.destination;

    if (is_allowed_multicast_destination(destination))
        return true;

    for (std::size_t port = 0; port < interfaces.size(); ++port) {
        if (has_interface_map[port] && interfaces[port].mac_address == destination)
            return true;
    }

    return false;
}
bool Device::is_frame_destination(std::size_t port, const Ethernet& frame) const {
    if (!frame.header.is_u64_mac_address(frame.header.destination))
        return false;
    if (!has_interface(port))
        return false;
    const auto destination = frame.header.destination;
    if (is_allowed_multicast_destination(destination))
        return true;
    if (interfaces[port].mac_address == destination)
        return true;
    return false;
}
bool Device::schedule_ethernet(std::size_t port, MACAddress destination, std::optional<std::uint16_t> vlan, EthType eth_type, std::vector<std::uint8_t> data, std::uint64_t scheduled_tick) {
    if (!has_interface(port))
        return false;
    Ethernet ethernet;
    ethernet.init();
    ethernet.header.set_source(interfaces[port].mac_address);
    ethernet.header.set_destination(destination);
    if (vlan.has_value())
        ethernet.header.push_front_vlan_id(*vlan);
    ethernet.header.set_eth_type(eth_type);
    ethernet.payload     = data;
    ethernet.tail.crc    = ethernet.compute_crc();
    DataRaw data_to_send = ethernet_to_data_raw(&ethernet);
    return interfaces[port].schedule(data_to_send, scheduled_tick);
}
bool Device::send_ethernet(std::size_t port, MACAddress destination, std::optional<std::uint16_t> vlan, EthType eth_type, std::vector<std::uint8_t> data) {
    return schedule_ethernet(port, destination, vlan, eth_type, data, global_tick);
}
bool Device::schedule_ethernet_broadcast(std::size_t port, std::optional<std::uint16_t> vlan, EthType eth_type, std::vector<std::uint8_t> data, std::uint64_t scheduled_tick) {
    return schedule_ethernet(port, 0xFFFFFFFFFFFF, vlan, eth_type, data, scheduled_tick);
}
bool Device::send_ethernet_broadcast(std::size_t port, std::optional<std::uint16_t> vlan, EthType eth_type, std::vector<std::uint8_t> data) {
    return send_ethernet(port, 0xFFFFFFFFFFFF, vlan, eth_type, data);
}
std::optional<Ethernet> Device::get_next_frame(std::size_t port) {
    Interface& interface  = interfaces[port];
    DataRaw& data         = interface.recv_queue.front();
    std::size_t data_size = data.size();

    interface.recv_queued_bytes -= data_size;
    Ethernet frame;
    frame.init();
    if (!frame.from_bytes(data.data(), data.size(), false))
        return std::nullopt;
    if (!frame.has_valid_crc())
        return std::nullopt;
    return frame;
}

// LLDP
bool Device::lldp_set_chassis_id(LLDP::ChassisIdSubtype chassis_id_type, std::vector<std::uint8_t> chassis_id) {
    this->chassis_id_type = chassis_id_type;
    this->chassis_id      = chassis_id;
    return true;
}
bool Device::lldp_set_chassis_id_mac(MACAddress mac_address) {
    std::vector<std::uint8_t> mac_address_unpacked(6);
    unpack_u48(mac_address_unpacked.data(), mac_address);
    return lldp_set_chassis_id(LLDP::ChassisIdSubtype::MACAddress, mac_address_unpacked);
}
bool Device::lldp_set_chassis_id_mac(const std::string& mac_address_str) {
    auto mac_address = str2mac(mac_address_str);
    if (mac_address.has_value())
        return lldp_set_chassis_id_mac(*mac_address);
    return false;
}

// Routing
bool Device::schedule_ipv4(std::size_t port, IPv4 packet, MACAddress destination, std::uint64_t scheduled_tick) {
    if (packet.size() > interfaces[port].mtu) {
        if (packet.header.get_df() == 1)
            return false;
        std::vector<IPv4> fragments = fragment_packet(packet, interfaces[port].mtu);
        for (const auto& fragment : fragments)
            schedule_ethernet(port, destination, std::nullopt, EthType::IP, ipv4_to_data_raw(&fragment), scheduled_tick);
    } else
        return schedule_ethernet(port, destination, std::nullopt, EthType::IP, ipv4_to_data_raw(&packet), scheduled_tick);
    return true;
}
bool Device::send_ipv4(std::size_t port, IPv4 packet, MACAddress destination) {
    return schedule_ipv4(port, packet, destination, global_tick);
}
