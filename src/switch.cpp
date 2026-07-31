#include "../include/switch.hpp"


MACTable::MACTable(): table({}), age_limit(settings.mac_aging_default), age_limit_int({}) {}
void MACTable::add_entry(MACAddress address, size_t interface_id) {
    auto it = table.find(address);
    if(it != table.end()) {
        it->second.age = 0;
        it->second.interface_id = interface_id;
    }
    else
        table[address] = (MACTableEntry){interface_id, 0};
}
void MACTable::set_age_limit(uint32_t age_limit) {
    this->age_limit = age_limit;
}
void MACTable::set_age_limit_int(size_t interface_id, uint32_t age_limit) {
    this->age_limit_int[interface_id] = age_limit;
}
void MACTable::update() {
    // Delete old entry
    for(auto it = table.begin(); it != table.end();) {
        MACTableEntry& entry = it->second;
        auto age_limit_it = age_limit_int.find(entry.interface_id);
        uint32_t age_limit_entry = age_limit_it == age_limit_int.end() ? age_limit : age_limit_it->second;
        if(entry.age >= age_limit_entry)
            it = table.erase(it);
        else {
            it->second.age++;
            it++;
        }
    }
}

Switch::Switch(): on(false), mac_table({}), bytes_budget(100e8) {
    SlotArrayInit(&interfaces, UINT16_MAX, sizeof(Interface));
}
void Switch::update() {
    if(!on)
        return;

    // Actually not used yet
    bytes_budget.update();
    // Update MAC table
    mac_table.update();
    // Update recieved data, no limit for the processing of incoming data
    for(size_t i = 0; i < interfaces.dataLen; i++) {
        if(!interfaces.active[i]) continue;
        Interface* in_interface = (Interface*)SlotArrayGet(&interfaces, i);
        while(!in_interface->recv_queue.empty()) {
            PacketRaw& packet = in_interface->recv_queue.front();
            ethernet_t ethernet;
            ethernet_from_bytes(&ethernet, packet.data(), packet.size(), true);
            // Learn from the source mac address of the packet
            mac_table.add_entry(ethernet.header.source, i);
            // Forward
            auto it = mac_table.table.find(ethernet.header.destination);
            if(it != mac_table.table.end()) {
                size_t out_interface_id = it->second.interface_id;
                if(!interfaces.active[out_interface_id]) [[unlikely]] {// Shouldn't happen
                    in_interface->recv_queue_pop();
                    continue;
                }
                Interface* out_interface = (Interface*)SlotArrayGet(&interfaces, out_interface_id);
                if(!out_interface->can_send(packet.size())) {
                    in_interface->recv_queue_pop();
                    continue;
                }
                out_interface->send_packet(std::move(packet));
                in_interface->recv_queue_pop();
            }
        }
    }
}

