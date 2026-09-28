#ifndef IPV4_HPP
#define IPV4_HPP

#include "ipv4header.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

class IPv4 {
public:
    IPv4() = default;

    IPv4Header header{};
    std::vector<std::uint8_t> data;

    void init();
    std::size_t size() const;

    void build_bytes(std::uint8_t* buffer) const;
    std::vector<std::uint8_t> to_bytes() const;
    bool from_bytes(const std::uint8_t* buffer, std::size_t buffer_size);

    /* Header Checksum */
    static std::uint16_t get_word(const IPv4Header& header, std::uint8_t index);
    std::uint16_t compute_header_checksum() const;
};

#endif
