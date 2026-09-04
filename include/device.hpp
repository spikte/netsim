#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "interface.hpp"
#include <string>

struct Device {
    std::string name;
    bool on;
    uint32_t bytes_per_tick;
    size_t process_interface_start;
    std::vector<Interface> interfaces;
    std::vector<bool> has_interface_map;
    std::unordered_map<std::string, size_t> name_to_port;
    std::unordered_map<MACAddress, size_t> mac_to_port;
    Device(int nport = 8);
    virtual ~Device() = default;

    virtual void set_name(std::string name);
    virtual size_t port_count() const;
    virtual size_t interface_count() const;
    virtual bool is_valid_port(size_t port) const;
    virtual bool has_interface(size_t port) const;
    virtual Interface* get_interface(size_t port);
    virtual Interface* get_interface(const std::string& name);
    virtual std::optional<size_t> get_interface_port(const std::string& name);
    virtual std::optional<std::string> interface_name(size_t port) const;
    virtual bool interface_add(size_t port, std::string name, MACAddress address, uint32_t mtu = 1500, size_t buffer_size = 1024 * 1024);
    virtual bool interface_del(size_t port);
    virtual bool interface_del(const std::string interface_name);
    virtual bool interface_turn_on(size_t port);
    virtual bool interface_turn_off(size_t port);
    virtual bool interface_turn_on(const std::string& interface_name);
    virtual bool interface_turn_off(const std::string& interface_name);
    virtual bool send_packet(size_t port, PacketRaw packet);
    virtual ssize_t process_packet_one(size_t port) = 0;
    virtual void update();
};
bool link_devices(Device& d1, size_t port1, Device& d2, size_t port2, Link& link);

#endif
