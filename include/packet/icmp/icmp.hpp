#ifndef ICMP_HPP
#define ICMP_HPP

#include "../ipv4/ipv4header.hpp"
#include "icmpheader.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

struct ICMP {
    ICMPHeader header;
    std::vector<std::uint8_t> data;

    void init();
    std::size_t size() const;
    void build_bytes(std::uint8_t* buffer) const;
    bool from_bytes(const std::uint8_t* buffer, std::size_t buffer_size);
    std::vector<std::uint8_t> to_bytes() const;

    std::uint16_t get_word(std::int16_t index) const;
    std::uint16_t compute_header_checksum() const;
};

#endif
