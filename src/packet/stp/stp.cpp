#include "../../../include/packet/stp/stp.hpp"

std::size_t BPDU::size() const {
    switch (static_cast<BPDUType>(bpdu_type)) {
        case BPDUType::Config:
            return 35;
        case BPDUType::TCN:
            return 4;
    }
}
std::uint16_t BPDU::get_bridge_priority() const {
    return bridge_id >> 48;
}
std::uint64_t BPDU::get_bridge_address() const {
    return bridge_id & 0xFFFFFFFFFFFF;
}
std::uint8_t BPDU::get_port_priority() const {
    return port_id >> 12;
}
std::uint16_t BPDU::get_port_number() const {
    return port_id & 0x0FFF;
}
void BPDU::set_port_priority(std::uint8_t priority) {
    priority &= 0x0F;
    port_id &= 0x0FFF;
    port_id |= ((std::uint16_t) priority << 12);
}
void BPDU::set_port_number(std::uint16_t port_number) {
    port_number &= 0x0FFF;
    port_id &= 0xF000;
    port_id |= port_number;
}
void BPDU::set_bridge_priority(std::uint16_t priority) {
    constexpr std::uint64_t priority_mask = ~((std::uint64_t) 1 << 48) - 1;
    bridge_id &= ~priority_mask;
    bridge_id |= ((std::uint64_t) priority << 48);
}
void BPDU::set_bridge_address(std::uint64_t mac_address) {
    constexpr std::uint64_t bridge_mask = ((std::uint64_t) 1 << 48) - 1;
    bridge_id &= ~bridge_mask;
    bridge_id |= mac_address & 0xFFFFFFFFFFFF;
}

std::size_t BPDU::build_bytes(std::uint8_t* buffer) const {
    std::size_t offset;

    offset = 0;
    unpack_u16_at(buffer, &offset, protocol_id);
    buffer[offset++] = version_id;
    buffer[offset++] = bpdu_type;
    if (static_cast<BPDUType>(bpdu_type) == BPDUType::TCN)
        return size();
    buffer[offset++] = flags;
    unpack_u64_at(buffer, &offset, root_id);
    unpack_u32_at(buffer, &offset, root_path_cost);
    unpack_u64_at(buffer, &offset, bridge_id);
    unpack_u16_at(buffer, &offset, port_id);
    unpack_u16_at(buffer, &offset, message_age);
    unpack_u16_at(buffer, &offset, max_age);
    unpack_u16_at(buffer, &offset, hello_time);
    unpack_u16_at(buffer, &offset, forward_delay);
    buffer[offset++] = version_1_len;
    unpack_u16_at(buffer, &offset, version_3_len);

    return size();
}
std::vector<std::uint8_t> BPDU::to_bytes() const {
    std::vector<std::uint8_t> buffer(size());

    build_bytes(buffer.data());

    return buffer;
}
bool BPDU::from_bytes(const std::uint8_t* buffer, std::size_t buffer_size) {
    if (!buffer || buffer_size < 4)
        return false;
    std::size_t offset;

    offset      = 0;
    protocol_id = pack_u16_at(buffer, &offset);
    version_id  = buffer[offset++];
    bpdu_type   = buffer[offset++];
    if (static_cast<BPDUType>(bpdu_type) == BPDUType::TCN)
        return true;
    else if (bpdu_type != 0)
        return false;
    flags          = buffer[offset++];
    root_id        = pack_u64_at(buffer, &offset);
    root_path_cost = pack_u32_at(buffer, &offset);
    bridge_id      = pack_u64_at(buffer, &offset);
    port_id        = pack_u16_at(buffer, &offset);
    message_age    = pack_u16_at(buffer, &offset);
    max_age        = pack_u16_at(buffer, &offset);
    hello_time     = pack_u16_at(buffer, &offset);
    forward_delay  = pack_u16_at(buffer, &offset);
    version_1_len  = buffer[offset++];
    version_3_len  = pack_u16_at(buffer, &offset);

    return true;
}
