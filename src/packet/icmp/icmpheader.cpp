#include "../../../include/packet/icmp/icmpheader.hpp"

void ICMPHeader::init() {
}
std::size_t ICMPHeader::size() const {
    return 8;
}
void ICMPHeader::set_type(ICMPType type) {
    this->type = static_cast<std::uint8_t>(type);
}
void ICMPHeader::set_code(ICMPCode code) {
    this->code = static_cast<std::uint8_t>(code);
}
void ICMPHeader::set_checksum(std::uint16_t checksum) {
    this->checksum = checksum;
}
void ICMPHeader::set_rest(std::uint32_t rest) {
    this->rest = rest;
}
void ICMPHeader::set_rest_byte(std::uint8_t byte, std::size_t index) {
    if (index >= 4)
        return;
    auto* rest_bytes  = reinterpret_cast<std::uint8_t*>(&this->rest);
    rest_bytes[index] = byte;
}
void ICMPHeader::set_rest_word(std::uint16_t word, std::size_t index) {
    if (index >= 2)
        return;
    auto* rest_words  = reinterpret_cast<std::uint16_t*>(&this->rest);
    rest_words[index] = word;
}
std::uint8_t ICMPHeader::get_rest_byte(std::size_t index) const {
    if (index >= 4)
        return 0; // I mean, use it well and it's not a weird return, maybe i should raise an exeption, TODO
    auto* rest_bytes = reinterpret_cast<const std::uint8_t*>(&this->rest);
    return rest_bytes[index];
}
std::uint16_t ICMPHeader::get_rest_word(std::size_t index) const {
    if (index >= 2)
        return 0;
    auto* rest_words = reinterpret_cast<const std::uint16_t*>(&this->rest);
    return rest_words[index];
}
void ICMPHeader::build_bytes(std::uint8_t* buffer) const {
    std::size_t offset = 0;

    buffer[offset++] = type;
    buffer[offset++] = code;
    unpack_u16_at(buffer, &offset, checksum);
    unpack_u32_at(buffer, &offset, rest);
}
bool ICMPHeader::from_bytes(const std::uint8_t* buffer, std::size_t buffer_size) {
    if (buffer_size < size())
        return false;
    init();
    std::size_t offset = 0;
    type               = buffer[offset++];
    code               = buffer[offset++];
    checksum           = pack_u16_at(buffer, &offset);
    rest               = pack_u32_at(buffer, &offset);

    return true;
}
std::vector<std::uint8_t> ICMPHeader::to_bytes() const {
    std::vector<std::uint8_t> buffer(size());

    build_bytes(buffer.data());

    return buffer;
}
