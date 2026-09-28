#include "../../../include/packet/ipv4/ipv4.hpp"
#include <cstring>

void IPv4::init() {
    header.init();
    data.clear();
}
std::size_t IPv4::size() const {
    return IPv4Header::size() + data.size();
}

void IPv4::build_bytes(std::uint8_t* buffer) const {
    header.build_bytes(buffer);
    if (!data.empty())
        std::memcpy(buffer + 20, data.data(), data.size());
}
std::vector<std::uint8_t> IPv4::to_bytes() const {
    std::vector<std::uint8_t> buffer(size());

    build_bytes(buffer.data());

    return buffer;
}
bool IPv4::from_bytes(const std::uint8_t* buffer, std::size_t buffer_size) {
    init();

    if (!header.from_bytes(buffer, buffer_size))
        return false;
    if (header.total_length < 20)
        return false;
    const std::size_t data_length = header.total_length - IPv4Header::size();
    if (buffer_size < IPv4Header::size() + data_length)
        return false;
    data.assign(buffer + IPv4Header::size(), buffer + IPv4Header::size() + data_length);

    return true;
}

std::uint16_t IPv4::get_word(const IPv4Header& header, std::uint8_t index) {
    switch (index) {
        case 0:
            return static_cast<std::uint16_t>((header.version_length << 8) | header.type_of_service);
        case 1:
            return header.total_length;
        case 2:
            return header.identification;
        case 3:
            return header.fragment_offset;
        case 4:
            return static_cast<std::uint16_t>(
                (header.time_to_live << 8) |
                header.protocol);
        case 5:
            return 0;
        case 6:
            return static_cast<std::uint16_t>(
                header.source >> 16);
        case 7:
            return static_cast<std::uint16_t>(
                header.source & 0xFFFF);
        case 8:
            return static_cast<std::uint16_t>(
                header.destination >> 16);
        case 9:
            return static_cast<std::uint16_t>(
                header.destination & 0xFFFF);
        default:
            return 0;
    }
}
std::uint16_t IPv4::compute_header_checksum() const {
    std::uint32_t checksum = 0;
    for (std::uint8_t i = 0; i < 10; ++i)
        checksum += get_word(header, i);
    while ((checksum & 0xFFFF0000U) != 0)
        checksum = (checksum & 0x0000FFFFU) + (checksum >> 16);
    return static_cast<std::uint16_t>(~checksum);
}
