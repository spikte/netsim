#include "../../include/sim/link.hpp"
#include "../../include/sim/device.hpp"
#include "../../include/sim/interface.hpp"

DataRaw ethernet_to_data_raw(const Ethernet* ethernet) {
    DataRaw data;

    data.resize(ethernet->size());
    ethernet->build_bytes(data.data(), false);

    return data;
}
DataRaw ipv4_to_data_raw(const IPv4* ipv4) {
    DataRaw data;

    data.resize(ipv4->size());
    ipv4->build_bytes(data.data());

    return data;
}

bool CompareScheduledData::operator()(const ScheduledData& a, const ScheduledData& b) const {
    if (a.scheduled_tick != b.scheduled_tick)
        return a.scheduled_tick > b.scheduled_tick;
    return a.sequence > b.sequence;
}

PPLinkUni::PPLinkUni(std::uint64_t propagation_speed) : propagation_speed(propagation_speed),
                                                        src(nullptr),
                                                        dst(nullptr) {}
Device* PPLinkUni::get_device_src() {
    if (!src)
        return nullptr;
    return src->device;
}
Device* PPLinkUni::get_device_dst() {
    if (!dst)
        return nullptr;
    return dst->device;
}
int64_t PPLinkUni::get_device_port_src() {
    if (!src)
        return -1;
    return src->device_port;
}
int64_t PPLinkUni::get_device_port_dst() {
    if (!dst)
        return -1;
    return dst->device_port;
}
bool PPLinkUni::connect_src(Device& device, std::size_t port) {
    if (src != nullptr)
        return false;
    if (!device.is_interface_connectable_egress(port))
        return false;

    src         = &device.interfaces[port];
    src->egress = this;

    return true;
}
bool PPLinkUni::connect_dst(Device& device, std::size_t port) {
    if (dst != nullptr)
        return false;
    if (!device.is_interface_connectable_ingress(port))
        return false;

    dst          = &device.interfaces[port];
    dst->ingress = this;

    return true;
}
bool PPLinkUni::connect(Device& device_src, std::size_t port_src, Device& device_dst, std::size_t port_dst) {
    if (!connect_src(device_src, port_src))
        return false;
    if (!connect_dst(device_dst, port_dst)) {
        disconnect_src();
        return false;
    }
    return true;
}
Interface* PPLinkUni::disconnect_src() {
    Interface* src;

    src = this->src;
    if (!src)
        return nullptr;
    // 1. Delete from the link vector in device and remove the entry from mac table
    if (src->device) {
        Device* device = src->device;
        if (device->ports.has_value() && device->mac_table.has_value() && dst) {
            for (std::uint16_t vlan_id : (*device->ports)[src->device_port].get_allowed_vlan())
                device->mac_table->del_entry(vlan_id, dst->mac_address);
        }
        device->links[src->device_port] = nullptr;
    }
    // 2. Clean in the interface
    src->egress = nullptr;
    // 3. Clean in PPLink
    this->src = nullptr;

    return src;
}
Interface* PPLinkUni::disconnect_dst() {
    Interface* dst;

    dst = this->dst;
    if (!dst)
        return nullptr;
    // 1. Delete from the link vector in device and remove entry from mac table
    if (dst->device) {
        Device* device = dst->device;
        if (device->ports.has_value() && device->mac_table.has_value() && src) {
            for (std::uint16_t vlan_id : (*device->ports)[dst->device_port].get_allowed_vlan())
                device->mac_table->del_entry(vlan_id, src->mac_address);
        }
        device->links[dst->device_port] = nullptr;
    }
    // 2. Clean in the interface
    dst->ingress = nullptr;
    // 3. Clean in PPLink
    this->dst = nullptr;

    return dst;
}
void PPLinkUni::disconnect() {
    disconnect_src();
    disconnect_dst();
}

// ==== Link ==== //
Link::Link(TwistedPairProperty property) : medium(MediumType::TwistedPair), twisted_pair_property(property) {
}
Link::Link(CoaxialProperty property) : medium(MediumType::Coaxial), coaxial_property(property) {}
Link::Link(FiberProperty property) : medium(MediumType::Fiber), fiber_property(property) {}
Link::~Link() {
    disconnect();
}
Interface* Link::get_interface_src(std::size_t index) {
    if (index != 0 && index != 1)
        return nullptr;
    return pplinks[index].src;
}
Interface* Link::get_interface_dst(std::size_t index) {
    if (index != 0 && index != 1)
        return nullptr;
    return pplinks[index].dst;
}
Device* Link::get_device_src(std::size_t index) {
    Interface* interface = get_interface_src(index);
    if (!interface)
        return nullptr;
    return interface->device;
}
Device* Link::get_device_dst(std::size_t index) {
    Interface* interface = get_interface_dst(index);
    if (!interface)
        return nullptr;
    return interface->device;
}
int64_t Link::get_device_port_src(std::size_t index) {
    Interface* interface = get_interface_src(index);
    if (!interface)
        return -1;
    return interface->device_port;
}
int64_t Link::get_device_port_dst(std::size_t index) {
    Interface* interface = get_interface_dst(index);
    if (!interface)
        return -1;
    return interface->device_port;
}
void Link::disconnect(std::size_t index) {
    if (index != 0 && index != 1)
        return;
    pplinks[index].disconnect();
}
void Link::disconnect() {
    disconnect(0);
    disconnect(1);
}
bool Link::connect(Device& device1, std::size_t port1, Device& device2, std::size_t port2) {
    if (!device1.has_interface(port1) || !device2.has_interface(port2))
        return false;
    Interface int1 = device1.interfaces[port1];
    Interface int2 = device2.interfaces[port2];

    //if (!(int1.mdi_type == MDIType::Auto || int2.mdi_type == MDIType::Auto)) {
    //    if (wiring == CableWiring::StraightThrough && int1.mdi_type != int2.mdi_type)
    //        return false;
    //    if (wiring == CableWiring::Crossover && int1.mdi_type == int2.mdi_type)
    //        return false;
    //}
    if (!pplinks[0].connect(device1, port1, device2, port2))
        goto error;
    if (!pplinks[1].connect(device2, port2, device1, port1))
        goto error;
    device1.links[port1] = this;
    device2.links[port2] = this;
    return true;
error:
    disconnect();
    return false;
}
