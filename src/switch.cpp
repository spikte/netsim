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
    has_interface                = true;
    return true;
}
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

// ==== Switch ====
size_t Switch::interface_count() const {
    int count;

    count = 0;
    for (const auto& port: ports)
        count += port.has_interface;

    return count;
}
bool Switch::has_interface(size_t slot) const {
    return slot < ports.size() && ports[slot].has_interface;
}
Interface& Switch::get_interface(size_t slot) {
    assert(slot < ports.size() && ports[slot].has_interface);
    return ports[slot].interface;
}
std::string Switch::interface_name(size_t slot) const {
    assert(slot < ports.size() && ports[slot].has_interface);
    return ports[slot].name;
}
std::optional<size_t> Switch::get_slot_by_name(const std::string& name) const {
    auto it = name_to_slot.find(name);
    if(it != name_to_slot.end())
        return it->second;
    return std::nullopt;
}

Switch::Switch(size_t nslot) : on(false), mac_table({}), bytes_per_tick(1024 * 1024), process_interface_start(0) {
    ports.resize(nslot);
}
bool Switch::int_add(size_t slot, MACAddress address, uint32_t mtu, size_t buffer_size) {
    if (slot >= ports.size() || ports[slot].has_interface)
        return false;
    ports[slot].add_interface(address, mtu, buffer_size);
    name_to_slot[ports[slot].name] = slot;
    mac_to_slot[address]           = slot;
    return true;
}
bool Switch::int_add(const std::string& name, MACAddress address, uint32_t mtu, size_t buffer_size) {
    auto it = name_to_slot.find(name);
    if (it == name_to_slot.end())
        return false;
    return int_add(it->second, address, mtu, buffer_size);
}
bool Switch::int_del(size_t slot) {
    if (slot >= ports.size() || !ports[slot].has_interface)
        return false;
    name_to_slot.erase(ports[slot].name);
    ports[slot].has_interface = false;
    return true;
}
bool Switch::int_del(const std::string& name) {
    auto it = name_to_slot.find(name);
    if (it == name_to_slot.end())
        return false;
    return int_del(it->second);
}
void Switch::int_turn_on(size_t slot) {
    if (slot >= ports.size() || !ports[slot].has_interface)
        return;
    ports[slot].interface.on = true;
}
void Switch::int_turn_off(size_t slot) {
    if (slot >= ports.size() || !ports[slot].has_interface)
        return;
    ports[slot].interface.on = false;
}
void Switch::int_turn_on(const std::string& name) {
    auto it = name_to_slot.find(name);
    if (it == name_to_slot.end())
        return;
    int_turn_on(it->second);
}
void Switch::int_turn_off(const std::string& name) {
    auto it = name_to_slot.find(name);
    if (it == name_to_slot.end())
        return;
    int_turn_off(it->second);
}
void Switch::int_set_type(size_t slot, SwitchPortType type) {
    if (slot >= ports.size())
        return;
    ports[slot].set_type(type);
}
void Switch::int_set_type(const std::string& name, SwitchPortType type) {
    auto it = name_to_slot.find(name);
    if (it == name_to_slot.end())
        return;
    int_set_type(it->second, type);
}
void Switch::int_add_vlan(size_t slot, uint16_t vlan) {
    if (slot >= ports.size())
        return;
    ports[slot].add_vlan(vlan);
}
void Switch::int_add_vlan(const std::string& name, uint16_t vlan) {
    auto it = name_to_slot.find(name);
    if (it != name_to_slot.end())
        int_add_vlan(it->second, vlan);
}
void Switch::int_rm_vlan(size_t slot, uint16_t vlan) {
    if (slot >= ports.size())
        return;
    ports[slot].rm_vlan(vlan);
}
void Switch::int_rm_vlan(const std::string& name, uint16_t vlan) {
    auto it = name_to_slot.find(name);
    if (it != name_to_slot.end())
        int_rm_vlan(it->second, vlan);
}
bool Switch::link(size_t slot, Switch& neighbor, size_t neighbor_slot, Link& link) {
    if (slot >= ports.size() || neighbor_slot >= neighbor.ports.size())
        return false;
    if (!ports[slot].has_interface || !neighbor.ports[neighbor_slot].has_interface)
        return false;
    Interface& interface          = ports[slot].interface;
    Interface& neighbor_interface = neighbor.ports[neighbor_slot].interface;
    return link_interface(interface, neighbor_interface, link);
}
bool Switch::link(const std::string& name, Switch& neighbor, const std::string& name_neighbor, Link& l) {
    auto it          = name_to_slot.find(name);
    auto it_neighbor = neighbor.name_to_slot.find(name_neighbor);

    if (it != name_to_slot.end() && it_neighbor != neighbor.name_to_slot.end())
        return link(it->second, neighbor, it_neighbor->second, l);
    return false;
}
bool Switch::send_packet(size_t slot, PacketRaw packet) {
    if (slot >= ports.size() || !ports[slot].has_interface)
        return false;

    ports[slot].interface.send_packet(std::move(packet));

    return true;
}
bool Switch::is_send_valid(size_t from_slot, size_t to_slot, const ethernet_t& ethernet) {
    if (ports[from_slot].type == SwitchPortType::ACCESS) {
        if (ethernet.header.has_tag)
            return false;
        return ports[to_slot].is_vlan_allowed(ports[from_slot].access_vlan);
    } else {
        uint16_t vlan;
        if (ethernet.header.has_tag)
            vlan = ethernet.header.tag & 0x0FFF;
        else
            vlan = ports[from_slot].native_vlan;
        return ports[to_slot].is_vlan_allowed(vlan) && ports[from_slot].is_vlan_allowed(vlan);
    }
    return false;
}
PacketRaw Switch::process_packet(size_t from_slot, size_t to_slot, ethernet_t& ethernet) {
    PacketRaw packet;
    bool is_from_trunk;
    bool is_to_trunk;

    is_from_trunk = ports[from_slot].type == SwitchPortType::TRUNK;
    is_to_trunk   = ports[to_slot].type == SwitchPortType::TRUNK;
    if (!is_from_trunk && is_to_trunk)
        ethernetheader_set_vlan(&ethernet.header, ports[from_slot].access_vlan);
    else if (is_from_trunk && !is_to_trunk)
        ethernetheader_set_has_tag(&ethernet.header, false);
    packet.resize(ethernet_size(&ethernet));
    ethernet_build_bytes(packet.data(), &ethernet, false);

    return packet;
}
bool Switch::send_packet_from(size_t from_slot, size_t to_slot, PacketRaw packet) {
    if (from_slot >= ports.size() || !ports[from_slot].has_interface)
        return false;
    if (to_slot >= ports.size() || !ports[to_slot].has_interface)
        return false;
    ethernet_t ethernet;
    PacketRaw packet_sent;

    ethernet_from_bytes(&ethernet, packet.data(), packet.size(), false);
    if (!is_send_valid(from_slot, to_slot, ethernet))
        return false;
    packet_sent = process_packet(from_slot, to_slot, ethernet);
    return send_packet(to_slot, packet_sent);
}
void Switch::broadcast(size_t from_slot, PacketRaw packet, uint16_t vlan) {
    for (size_t i = 0; i < ports.size(); i++) {
        if (!ports[i].has_interface || i == from_slot)
            continue;
        send_packet_from(from_slot, i, packet);
    }
}
ssize_t Switch::process_packet_one(size_t slot) {
    if (!ports[slot].has_interface || ports[slot].interface.recv_queued_bytes == 0)
        return -1;
    Interface& interface = ports[slot].interface;
    PacketRaw& packet    = interface.recv_queue.front();
    size_t packet_size   = packet.size();
    uint16_t vlan;

    // We remove the size to recv_queued_bytes
    // now because then we move the data so we won't
    // be able to access the packet size
    interface.recv_queued_bytes -= packet.size();

    // Parse the packet ethernet header
    ethernet_t ethernet;
    ethernet_from_bytes(&ethernet, packet.data(), packet.size(), false);
    // Retrieve VLAN tag
    if (ports[slot].type == SwitchPortType::ACCESS)
        vlan = ports[slot].access_vlan;
    else {
        if (!ethernet.header.has_tag)
            vlan = ports[slot].native_vlan;
        else
            vlan = ethernet.header.tag & (uint32_t) 0x0FFF;
    }

    // Learn the source address port
    mac_table.add_entry(vlan, ethernet.header.source, slot);

    // If packet has arrived at destination process it
    if (mac_to_slot.find(ethernet.header.destination) != mac_to_slot.end()) {
        // TODO: Process packet
        interface.recv_queue.pop_front();
        return packet_size;
    }

    // Else forward it
    MACTableKey mac_table_key = {vlan, ethernet.header.destination};
    auto it                   = mac_table.table.find(mac_table_key);
    if (it != mac_table.table.end())
        send_packet_from(slot, it->second.interface_id, std::move(packet));
    else
        broadcast(slot, std::move(packet), vlan);

    // Pop the packet from the interface sending it
    interface.recv_queue.pop_front();

    return packet_size;
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
            size_t slot                 = (process_interface_start + i) % ports.size();
            ssize_t process_packet_size = process_packet_one(slot);
            if (process_packet_size < 0)
                continue;
            progress = true;
            budget -= process_packet_size;
        }
    }
    process_interface_start = (process_interface_start + 1) % ports.size();
}
