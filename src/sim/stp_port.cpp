#include "../../include/sim/stp_port.hpp"
#include "../../include/sim/device.hpp"

/* STPPort */
STPPort::STPPort(Interface* interface) : interface(interface) {
    last_send = global_tick;
}
void STPPort::enable() {
    if (!enabled)
        last_send = global_tick;
    enabled = true;
}
void STPPort::disable() {
    enabled = false;
}
std::uint16_t STPPort::port_id() {
    if (!interface)
        return 0;
    std::uint16_t port_id = interface->device_port;
    port_id |= (std::uint16_t) port_priority << 12;

    return port_id;
}
void STPPort::set_cost_short() {
    if (!interface)
        return;
    switch (interface->bits_per_second) {
        case Mbs(10):
            cost = 100;
            break;
        case Mbs(100):
            cost = 19;
            break;
        case Gbs(1):
            cost = 4;
            break;
        case Gbs(10):
            cost = 2;
            break;
    }
}
void STPPort::set_cost_long() {
    if (!interface)
        return;
    switch (interface->bits_per_second) {
        case Mbs(10):
            cost = 2'000'000;
            break;
        case Mbs(100):
            cost = 200'000;
            break;
        case Gbs(1):
            cost = 20'000;
            break;
        case Gbs(10):
            cost = 2'000;
            break;
        case Gbs(100):
            cost = 200;
            break;
        case Tbs(1):
            cost = 20;
            break;
    }
}

/* STPDevice */
STPDevice::STPDevice(Device* device) : device(device) {
    root_id = bridge_id();
}
std::uint64_t STPDevice::bridge_id() {
    if (!device)
        return 0;
    Interface* interface = nullptr;
    std::size_t i        = 0;
    while (!device->has_interface(i)) {
        interface = &device->interfaces[i];
        i++;
    }
    if (!interface)
        return 0;
    MACAddress address      = interface->mac_address;
    std::uint64_t bridge_id = (std::uint64_t) priority << 48 | address;

    return bridge_id;
}
void STPDevice::set_cost() {
    for(std::size_t i = 0; i < ports.size(); i++) {
        if(!device->has_interface(i))
            continue;
        if (version == STPVersion::STP)
            ports[i].set_cost_short();
        else
            ports[i].set_cost_long();
    }
}
void STPDevice::update_root(std::size_t port) {
    STPPort stp_port = ports[port];
    if(*stp_port.received_root_id > root_id)
        return;
    if(*stp_port.received_root_id < root_id) {
        root_id = *stp_port.received_root_id;
    }
}
