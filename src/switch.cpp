#include "../include/switch.hpp"

// ==== MACTable ====
MACTable::MACTable() : table({}), age_limit(settings.mac_aging_default), age_limit_int({}) {}
bool MACTableKey::operator==(const MACTableKey& other) const {
    return vlan == other.vlan &&
           address == other.address;
}
void MACTable::add_entry(uint16_t vlan, MACAddress address, size_t interface_id) {
    MACTableKey key          = MACTableKey{vlan, address};
    auto it                  = table.find(key);
    auto age_limit_it        = age_limit_int.find(interface_id);
    uint64_t age_limit_entry = age_limit_it == age_limit_int.end() ? age_limit : age_limit_it->second;

    if (it != table.end()) {
        it->second.aging_tick   = global_tick + age_limit_entry;
        it->second.interface_id = interface_id;
    } else
        table[key] = MACTableEntry{interface_id, global_tick + age_limit_entry};
}
void MACTable::set_age_limit(uint32_t age_limit) {
    this->age_limit = age_limit;
}
void MACTable::set_age_limit_int(size_t interface_id, uint32_t age_limit) {
    this->age_limit_int[interface_id] = age_limit;
}
void MACTable::update() {
    // Delete old entry
    for (auto it = table.begin(); it != table.end();) {
        if (global_tick < it->second.aging_tick)
            it++;
        else
            it = table.erase(it);
    }
}

// === SwitchPort ====
SwitchPort::SwitchPort(std::string name) : name(name), has_interface(false), type(SwitchPortType::ACCESS), access_vlan(1), native_vlan(1) {}
bool SwitchPort::add_interface(MACAddress address, uint32_t mtu, size_t buffer_size) {
    interface.address     = address;
    interface.on          = false;
    interface.mtu         = mtu;
    interface.to          = nullptr;
    interface.from        = nullptr;
    interface.buffer_size = buffer_size;
    interface.send_queue.clear();
    interface.recv_queue.clear();
    interface.send_queued_bytes = 0;
    interface.recv_queued_bytes = 0;
    has_interface               = true;
}

// ==== Switch ====
Switch::Switch(int nslot) : nslot(nslot), on(false), mac_table({}), bytes_per_tick(1024 * 1024), process_interface_start(0) {
    ports.resize(nslot);
}
bool Switch::add_interface(size_t slot, MACAddress address, uint32_t mtu, size_t buffer_size) {
    if (slot >= nslot || ports[slot].has_interface)
        return false;
    ports[slot].add_interface(address, mtu, buffer_size);
    name_to_interface[ports[slot].name] = slot;
    return true;
}
bool Switch::del_interface(size_t slot) {
    if (slot >= nslot || !ports[slot].has_interface)
        return false;
    name_to_interface.erase(ports[slot].name);
    ports[slot].has_interface = false;
    return true;
}
void Switch::turn_on_interface(size_t slot) {
    if (slot >= nslot || !ports[slot].has_interface)
        return;
    ports[slot].interface.on = true;
}
void Switch::turn_off_interface(size_t slot) {
    if (slot >= nslot || !ports[slot].has_interface)
        return;
    ports[slot].interface.on = false;
}
void Switch::turn_on_interface(std::string name) {
    auto it = name_to_interface.find(name);
    if (it == name_to_interface.end())
        return;
    turn_on_interface(it->second);
}
void Switch::turn_off_interface(std::string name) {
    auto it = name_to_interface.find(name);
    if (it == name_to_interface.end())
        return;
    turn_off_interface(it->second);
}
bool Switch::link(size_t slot, Switch& neighbor, size_t neighbor_slot, Link& link) {
    if (slot >= nslot || neighbor_slot >= neighbor.nslot)
        return false;
    if (!ports[slot].has_interface || !neighbor.ports[neighbor_slot].has_interface)
        return false;
    Interface& interface          = ports[slot].interface;
    Interface& neighbor_interface = neighbor.ports[neighbor_slot].interface;
    if (!link[0].connect_src(&interface))
        goto error;
    if (!link[0].connect_dst(&neighbor_interface))
        goto error;
    if (!link[1].connect_src(&neighbor_interface))
        goto error;
    if (!link[1].connect_dst(&interface))
        goto error;
    return true;
error:
    if (link[0].src)
        link[0].disconnect_src();
    if (link[0].dst)
        link[0].disconnect_dst();
    if (link[1].src)
        link[1].disconnect_src();
    if (link[1].dst)
        link[1].disconnect_dst();
    return false;
}
bool Switch::send_packet(size_t slot, PacketRaw packet) {
    if (slot >= nslot || !ports[slot].has_interface)
        return false;

    ports[slot].interface.send_packet(std::move(packet));

    return true;
}
void Switch::broadcast(PacketRaw packet, size_t from_slot) {
    for (size_t i = 0; i < ports.size(); i++) {
        if (!ports[i].has_interface || i == from_slot)
            continue;
        send_packet(i, packet); // We don't move `packet` here because we want to make a copy of the packet for each interface
    }
}
ssize_t Switch::process_packet_one(size_t slot) {
    if (!ports[slot].has_interface || ports[slot].interface.recv_queued_bytes == 0)
        return -1;
    Interface& interface = ports[slot].interface;
    PacketRaw& packet    = interface.recv_queue.front();

    // We remove the size to recv_queued_bytes
    // now because then we move the data so we won't
    // be able to access the packet size
    interface.recv_queued_bytes -= packet.size();

    // 1. Check if

    // Learn the destination address
    ethernet_t ethernet;
    ethernet_from_bytes(&ethernet, packet.data(), packet.size(), false);
    mac_table.add_entry(ethernet.header.source, slot);

    // Forward
    auto it = mac_table.table.find(ethernet.header.destination);
    if (it != mac_table.table.end())
        send_packet(it->second.interface_id, std::move(packet));
    else
        broadcast(std::move(packet), interface_id);

    // Pop the packet from the interface sending it
    in_interface.recv_queue.pop_front();
}
void Switch::update() {
    if (!on)
        return;

    // Update MAC table
    mac_table.update();
    // Update recieved data, no limit for the processing of incoming data
    int64_t budget = bytes_per_tick;
    bool progress  = true;
    while (budget > 0 && progress) {
        progress = false;
        for (size_t i = 0; i < ports.size(); i++) {
            size_t slot = (process_interface_start + i) % ports.size();
            ssize_t process_packet_size = process_packet_one(slot);
            if(process_packet_size < 0)
                continue;
            progress = true;
            budget -= process_packet_size;
        }
    }
    process_interface_start = (process_interface_start + 1) % ports.size();
}
