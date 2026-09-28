#ifndef ETHERNET_HPP
#define ETHERNET_HPP

#include "ethernetheader.hpp"
extern "C" {
#include "../crc/crc.h"
}

#include <cstddef>
#include <cstdint>
#include <vector>

struct Ethernet {
    EthernetHeader header;
    std::vector<std::uint8_t> payload;
    EthernetTail tail;

    Ethernet();
    void init();
    std::size_t size() const;

    std::uint32_t compute_crc() const;
    bool has_valid_crc() const;

    std::size_t build_bytes(std::uint8_t* buffer, bool preamble = false) const;
    std::vector<std::uint8_t> to_bytes(bool preamble = false) const;
    bool from_bytes(const std::uint8_t* buffer, std::size_t buffer_size, bool preamble = false);
};

#endif
