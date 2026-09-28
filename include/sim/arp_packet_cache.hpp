#ifndef ARP_PACKET_CACHE_HPP
#define ARP_PACKET_CACHE_HPP

#include <unordered_map>
#include "../packet/packet.hpp"
#include "type.hpp"
#include "interface.hpp"

struct ARPPacketCacheKey {
    std::size_t port;
    IPv4Address address;

    bool operator==(const ARPPacketCacheKey& other) const;
};
struct ARPPacketCacheValue {
    IPv4 packet;
    std::uint64_t scheduled_tick;
};
struct ARPPacketCacheKeyHash {
    std::size_t operator()(const ARPPacketCacheKey& key) const;
};
using ARPPacketCache = std::unordered_map<ARPPacketCacheKey, std::vector<ARPPacketCacheValue>, ARPPacketCacheKeyHash>;

#endif
