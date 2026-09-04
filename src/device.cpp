#include "../include/device.hpp"

Device::Device(int nport) : on(true), bytes_per_tick(1024 * 1024), process_interface_start(0) {
    interfaces.resize(nport);
    has_interface_map.resize(nport, false);
}
void Device::set_name(std::string name) {
    this->name = name;
}
size_t Device::port_count() const {
    return interfaces.size();
}
size_t Device::interface_count() const {
    size_t count;

    count = 0;
    for (size_t i = 0; i < has_interface_map.size(); i++)
        count += has_interface_map[i];

    return count;
}
bool Device::is_valid_port(size_t port) const {
    return port < interfaces.size();
}
bool Device::has_interface(size_t port) const {
    return is_valid_port(port) && has_interface_map[port];
}
Interface* Device::get_interface(size_t port) {
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
std::optional<size_t> Device::get_interface_port(const std::string& name) {
    auto it = name_to_port.find(name);
    if (it != name_to_port.end())
        return it->second;
    return std::nullopt;
}
std::optional<std::string> Device::interface_name(size_t port) const {
    if (has_interface(port))
        return interfaces[port].name;
    return std::nullopt;
}
bool Device::interface_add(size_t port, std::string name, MACAddress address, uint32_t mtu, size_t buffer_size) {
    if (!is_valid_port(port))
        return false;
    interfaces[port].name        = name;
    interfaces[port].address     = address;
    interfaces[port].mtu         = mtu;
    interfaces[port].buffer_size = buffer_size;
    interfaces[port].to          = nullptr;
    interfaces[port].from        = nullptr;
    interfaces[port].send_queue.clear();
    interfaces[port].recv_queue.clear();
    interfaces[port].send_queued_bytes = 0;
    interfaces[port].recv_queued_bytes = 0;
    name_to_port[name]                 = port;
    has_interface_map[port]            = true;
    return true;
}
bool Device::interface_del(size_t port) {
    if (port >= has_interface_map.size() || !has_interface_map[port])
        return false;
    has_interface_map[port] = false;

    return true;
}
bool Device::interface_del(const std::string interface_name) {
    auto it = name_to_port.find(interface_name);
    if (it != name_to_port.end())
        return interface_del(it->second);
    return false;
}
bool Device::interface_turn_on(size_t port) {
    if (!has_interface(port) || interfaces[port].on)
        return false;
    interfaces[port].on = true;

    return true;
}
bool Device::interface_turn_off(size_t port) {
    if (!has_interface(port) || interfaces[port].on)
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
bool Device::send_packet(size_t port, PacketRaw packet) {
    if (!has_interface(port))
        return false;

    interfaces[port].send_packet(std::move(packet));

    return true;
}
void Device::update() {
    if (!on)
        return;

    // Update recieved data, no limit for the processing of incoming data
    int64_t budget    = bytes_per_tick;
    bool progress     = true;
    while (budget > 0 && progress) {
        progress = false;
        for (size_t i = 0; i < port_count(); i++) {
            if(!has_interface(i))
                continue;
            size_t port                 = (process_interface_start + i) % port_count();
            ssize_t process_packet_size = process_packet_one(port);
            if (process_packet_size < 0)
                continue;
            progress = true;
            budget -= process_packet_size;
        }
    }
    process_interface_start = (process_interface_start + 1) % port_count();
}

bool link_devices(Device& d1, size_t port1, Device& d2, size_t port2, Link& link) {
    if (!d1.has_interface(port1) || !d2.has_interface(port2))
        return false;
    return link_interface(*d1.get_interface(port1), *d2.get_interface(port2), link);
}
