#include "../../include/sim/packet_reassembly.hpp"

IPv4 PacketReassemblyValue::reassemble() {
    std::sort(packets.begin(), packets.end(), [](const IPv4& left, const IPv4& right) {
        return left.header.fragment_offset < right.header.fragment_offset;
    });
    IPv4 packet;

    packet.init();
    packet.header.set_version(4);
    packet.header.set_header_length(20); // No option, TODO: implement option
    packet.header.type_of_service = packets[0].header.type_of_service;
    packet.header.set_df(1);
    packet.header.set_mf(0);
    packet.header.set_fragment_offset(0);
    packet.header.set_identification(0);
    packet.header.set_time_to_live(0);
    packet.header.set_protocol(packets[0].header.protocol);
    packet.header.source      = packets[0].header.source;
    packet.header.destination = packets[0].header.destination;
    for (auto fragment : packets)
        packet.data.insert(packet.data.end(), fragment.data.begin(), fragment.data.end());
    return packet;
}
PacketReassemblyKey::PacketReassemblyKey(const IPv4& packet) : source(packet.header.source),
                                                               destination(packet.header.destination),
                                                               protocol(packet.header.protocol),
                                                               identification(packet.header.identification) {}
bool PacketReassemblyKey::operator==(const PacketReassemblyKey& other) const {
    return source == other.source &&
           destination == other.destination &&
           protocol == other.protocol &&
           identification == other.identification;
}
void PacketReassemblyTable::add_packet(Ethernet& frame, IPv4& packet) {
    PacketReassemblyKey key(packet);
    auto it = table.find(key);
    if (it != table.end()) {
        if (it->second.done)
            return;
        std::vector<IPv4>& packets = it->second.packets;
        if (packet.header.type_of_service != packets[0].header.type_of_service)
            return;
        packets.push_back(std::move(packet));
        it->second.aging_tick = global_tick + timeout;
        if (packet.header.get_mf())
            it->second.done = true;
    } else {
        table[key] = PacketReassemblyValue({.packets    = {packet},
                                            .aging_tick = global_tick + timeout,
                                            .done       = false});
    }
}
void PacketReassemblyTable::update() {
    for (auto it = table.begin(); it != table.end();) {
        if (it->second.done) {
            to_process.push(it->second);
            it = table.erase(it);
        } else if (it->second.aging_tick <= global_tick)
            it = table.erase(it);
        else
            it++;
    }
}
std::vector<IPv4> fragment_packet(const IPv4& packet, std::uint32_t mtu) {
    if (mtu < 20)
        return {};
    std::vector<IPv4> fragments;
    std::size_t nfragment;

    std::size_t payload_size = mtu - 20;
    if (payload_size == 0)
        return {};
    nfragment = (packet.data.size() + payload_size - 1) / payload_size;
    fragments.resize(nfragment);
    for (std::size_t i = 0; i < nfragment; i++) {
        IPv4& fragment = fragments[i];
        memcpy((void*) &fragment, (void*) &packet, sizeof(IPv4));

        // Setup payload
        std::size_t start     = i * payload_size;
        std::size_t remaining = packet.data.size() - start;
        std::size_t this_len  = std::min(payload_size, remaining);
        fragment.data         = std::vector<std::uint8_t>(packet.data.begin() + start, packet.data.begin() + start + this_len);

        // Modify header and recompute checksum
        bool is_last = i == nfragment - 1;
        fragment.header.set_mf(is_last ? 0 : 1);
        fragment.header.set_fragment_offset(i);
        fragment.header.set_total_length(20 + this_len);
        fragment.header.checksum = fragment.compute_header_checksum();
    }
    return fragments;
}
