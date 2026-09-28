#ifndef PACKET_REASSEMBLY_HPP
#define PACKET_REASSEMBLY_HPP

#include "../packet/packet.hpp"
#include "type.hpp"
#include <queue>

struct PacketReassemblyKey {
    PacketReassemblyKey(const IPv4& packet);
    IPv4Address source;
    IPv4Address destination;
    std::uint8_t protocol;
    std::uint16_t identification;
    bool operator==(const PacketReassemblyKey& other) const;
};
template <>
struct std::hash<PacketReassemblyKey> {
    std::size_t operator()(const PacketReassemblyKey& key) const {
        std::size_t h;

        h = 0;
        h ^= std::hash<IPv4Address>{}(key.source);
        h ^= std::hash<IPv4Address>{}(key.destination);
        h ^= std::hash<std::uint8_t>{}(key.protocol);
        h ^= std::hash<std::uint16_t>{}(key.identification);

        return h;
    }
};
struct PacketReassemblyValue {
    std::vector<IPv4> packets;
    Ethernet frame;
    std::uint64_t aging_tick;
    bool done;
    IPv4 reassemble();
};
struct PacketReassemblyTable {
    std::uint64_t timeout;
    std::unordered_map<PacketReassemblyKey, PacketReassemblyValue> table;
    std::queue<PacketReassemblyValue> to_process;
    void add_packet(Ethernet& frame, IPv4& packet);
    void update();
};
std::vector<IPv4> fragment_packet(const IPv4& packet, std::uint32_t mtu);

#endif
