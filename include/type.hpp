#ifndef NETSIM_TYPE_HPP
#define NETSIM_TYPE_HPP

#include <cstdint>
#include <vector>

using MACAddress = uint64_t;
using PacketRaw  = std::vector<uint8_t>;
using IPv4Address = uint32_t;

#endif
