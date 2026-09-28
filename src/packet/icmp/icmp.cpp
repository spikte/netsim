#include "../../../include/packet/icmp/icmp.hpp"
#include <cstring>

void ICMP::init() {
    header.init();
}
std::size_t ICMP::size() const {
    return header.size() + data.size();
}
void ICMP::build_bytes(std::uint8_t* buffer) const {
    header.build_bytes(buffer);
    if (!data.empty())
        std::memcpy(buffer + header.size(), data.data(), data.size());
}
bool ICMP::from_bytes(const std::uint8_t* buffer, std::size_t buffer_size) {
    std::size_t accumulated_size;

    accumulated_size = 0;
    if (!header.from_bytes(buffer, buffer_size))
        return false;
    accumulated_size = header.size();
    data.assign(buffer + accumulated_size, buffer + buffer_size);

    return true;
}
std::vector<std::uint8_t> ICMP::to_bytes() const {
    std::vector<std::uint8_t> buffer(size());

    build_bytes(buffer.data());

    return buffer;
}

std::uint16_t ICMP::compute_header_checksum() const {
    std::uint32_t sum = 0;

    sum += (static_cast<std::uint16_t>(header.type) << 8) |
           static_cast<std::uint16_t>(header.code);
    sum += static_cast<std::uint16_t>(header.rest >> 16);
    sum += static_cast<std::uint16_t>(header.rest & 0xFFFF);
    for (std::size_t i = 0; i < data.size(); i += 2) {
        std::uint16_t word = static_cast<std::uint16_t>(data[i]) << 8;
        if (i + 1 < data.size())
            word |= data[i + 1];
        sum += word;
    }
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return static_cast<std::uint16_t>(~sum);
}
