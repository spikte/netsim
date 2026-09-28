#include "../../include/sim/arp_packet_cache.hpp"

bool ARPPacketCacheKey::operator==(const ARPPacketCacheKey& other) const {
    return port == other.port &&
           address == other.address;
}
std::size_t ARPPacketCacheKeyHash::operator()(const ARPPacketCacheKey& key) const {
    std::size_t h1 = std::hash<std::size_t>{}(key.port);
    std::size_t h2 = std::hash<IPv4Address>{}(key.address);

    return h1 ^ (h2 << 1);
}
