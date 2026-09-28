#include "../../../include/packet/ethernet/ethernet.hpp"

#include <cstring>

Ethernet::Ethernet() {
    init();
}
void Ethernet::init() {
    header.init();
    payload.clear();
    tail.init();
}
std::size_t Ethernet::size() const {
    return header.size() + payload.size() + 4;
}

bool Ethernet::from_bytes(const std::uint8_t* buffer, std::size_t buffer_size, bool preamble) {
    init();

    if (!header.from_bytes(buffer, buffer_size, preamble))
        return false;
    std::size_t header_size = header.size();
    if (preamble)
        header_size += 8;
    if (buffer_size < header_size + 4)
        return false;
    std::size_t payload_size = static_cast<std::uint16_t>(buffer_size - header_size - 4);
    payload.assign(buffer + header_size, buffer + header_size + payload_size);
    tail.crc = pack_u32(buffer + buffer_size - 4);

    return true;
}
std::size_t Ethernet::build_bytes(std::uint8_t* buffer, bool preamble) const {
    std::size_t offset = 0;
    offset             = header.build_bytes(buffer, preamble);
    if (!payload.empty()) {
        std::memcpy(
            buffer + offset,
            payload.data(),
            payload.size());

        offset += payload.size();
    }
    unpack_u32_at(buffer, &offset, tail.crc);

    return offset;
}
std::vector<std::uint8_t> Ethernet::to_bytes(bool preamble) const {
    std::size_t buffer_size = size();

    if (preamble)
        buffer_size += 8;
    std::vector<std::uint8_t> buffer(buffer_size);
    build_bytes(buffer.data(), preamble);

    return buffer;
}

std::uint32_t Ethernet::compute_crc() const {
    const std::vector<std::uint8_t> buffer = to_bytes(false);
    if (buffer.size() < 4)
        return 0;
    return crc32_impl(0, reinterpret_cast<const char*>(buffer.data()), buffer.size() - 4);
}
bool Ethernet::has_valid_crc() const {
    const std::uint32_t fcs = compute_crc();
    if (fcs == 0)
        return false;

    return fcs == tail.crc;
}
