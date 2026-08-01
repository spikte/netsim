#include "../include/switch.hpp"

MACTable::MACTable() : table({}), age_limit(settings.mac_aging_default), age_limit_int({}) {}
void MACTable::add_entry(MACAddress address, size_t interface_id) {
    auto it                  = table.find(address);
    auto age_limit_it        = age_limit_int.find(interface_id);
    uint64_t age_limit_entry = age_limit_it == age_limit_int.end() ? age_limit : age_limit_it->second;

    if (it != table.end()) {
        it->second.aging_tick   = global_tick + age_limit_entry;
        it->second.interface_id = interface_id;
    } else
        table[address] = MACTableEntry{interface_id, global_tick + age_limit_entry};
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
        if(global_tick < it->second.aging_tick)
            it++;
        else
            it = table.erase(it);
    }
}
Switch::Switch(int nslot) : nslot(nslot), on(false), mac_table({}), bytes_per_tick(1024 * 1024), process_interface_start(0) {
    interfaces.resize(nslot);
    slot_has_interface.resize(nslot, false);
}
bool Switch::add_interface(size_t slot, MACAddress address, std::string name, uint32_t mtu, size_t buffer_size) {
    if (slot >= nslot || slot_has_interface[slot])
        return false;
    interfaces[slot].address     = address;
    interfaces[slot].name        = name;
    interfaces[slot].on          = false;
    interfaces[slot].mtu         = mtu;
    interfaces[slot].to          = nullptr;
    interfaces[slot].from        = nullptr;
    interfaces[slot].buffer_size = buffer_size;
    interfaces[slot].send_queue.clear();
    interfaces[slot].recv_queue.clear();
    interfaces[slot].send_queued_bytes = 0;
    interfaces[slot].recv_queued_bytes = 0;
    name_to_interface[name]            = slot;
    slot_has_interface[slot]           = true;
    return true;
}
bool Switch::del_interface(size_t slot) {
    if (slot >= nslot || !slot_has_interface[slot])
        return false;
    name_to_interface.erase(interfaces[slot].name);
    slot_has_interface[slot] = false;
    return true;
}
void Switch::turn_on_interface(size_t slot) {
    if (slot >= nslot || !slot_has_interface[slot])
        return;
    interfaces[slot].on = true;
}
void Switch::turn_off_interface(size_t slot) {
    if (slot >= nslot || !slot_has_interface[slot])
        return;
    interfaces[slot].on = false;
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
    if (!slot_has_interface[slot] || !neighbor.slot_has_interface[neighbor_slot])
        return false;
    Interface& interface          = interfaces[slot];
    Interface& neighbor_interface = neighbor.interfaces[neighbor_slot];
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
    if (slot >= nslot || !slot_has_interface[slot])
        return false;

    interfaces[slot].send_packet(std::move(packet));

    return true;
}
void Switch::broadcast(PacketRaw packet, size_t from_slot) {
    for (size_t i = 0; i < interfaces.size(); i++) {
        if (!slot_has_interface[i] || i == from_slot)
            continue;
        send_packet(i, packet); // We don't move `packet` here because we want to make a copy of the packet for each interface
    }
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
        for (size_t i = 0; i < interfaces.size(); i++) {
            size_t interface_id = (process_interface_start + i) % interfaces.size();
            if (!slot_has_interface[interface_id] || interfaces[interface_id].recv_queued_bytes == 0)
                continue;
            progress                = true;
            Interface& in_interface = interfaces[interface_id];
            PacketRaw& packet       = in_interface.recv_queue.front();
            budget -= packet.size();
            // We remove the size to recv_queued_bytes
            // now because then we move the data so we won't
            // be able to access the packet size
            in_interface.recv_queued_bytes -= packet.size();

            // Learn the destination address
            ethernet_t ethernet;
            ethernet_from_bytes(&ethernet, packet.data(), packet.size(), false);
            mac_table.add_entry(ethernet.header.source, interface_id);

            // Forward
            auto it = mac_table.table.find(ethernet.header.destination);
            if (it != mac_table.table.end())
                send_packet(it->second.interface_id, std::move(packet));
            else
                broadcast(std::move(packet), interface_id);

            // Pop the packet from the interface sending it
            in_interface.recv_queue.pop_front();
        }
    }
    process_interface_start = (process_interface_start + 1) % interfaces.size();
}
