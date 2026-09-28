#include "../../../include/packet/ipv4/ipv4header.hpp"

#include <cstdio>

/* enum2str */
const char* ipv4header_tosp2str(IPv4TOSPrecedence precedence) {
    switch (precedence) {
        case IPv4TOSPrecedence::NetworkControl:
            return "NetworkControl";
        case IPv4TOSPrecedence::InternetworkControl:
            return "InternetworkControl";
        case IPv4TOSPrecedence::CRITIC:
            return "CRITIC";
        case IPv4TOSPrecedence::FlashOverride:
            return "FlashOverride";
        case IPv4TOSPrecedence::Flash:
            return "Flash";
        case IPv4TOSPrecedence::Immediate:
            return "Immediate";
        case IPv4TOSPrecedence::Priority:
            return "Priority";
        case IPv4TOSPrecedence::Routine:
            return "Routine";
        default:
            return "None";
    }
}
const char* ipv4header_protocol2str(IPv4Protocol protocol) {
    switch (protocol) {
        case IPv4Protocol::ICMP:
            return "ICMP";
        case IPv4Protocol::GatewayToGateway:
            return "GatewayToGateway";
        case IPv4Protocol::CMCCGatewayMonitoringMessage:
            return "CMCCGatewayMonitoringMessage";
        case IPv4Protocol::ST:
            return "ST";
        case IPv4Protocol::TCP:
            return "TCP";
        case IPv4Protocol::UCL:
            return "UCL";
        case IPv4Protocol::Secure:
            return "Secure";
        case IPv4Protocol::BBNRCCMonitoring:
            return "BBNRCCMonitoring";
        case IPv4Protocol::NVP:
            return "NVP";
        case IPv4Protocol::PUP:
            return "PUP";
        case IPv4Protocol::Pluribus:
            return "Pluribus";
        case IPv4Protocol::Telenet:
            return "Telenet";
        case IPv4Protocol::XNET:
            return "XNET";
        case IPv4Protocol::Chaos:
            return "Chaos";
        case IPv4Protocol::UserDatagram:
            return "UserDatagram";
        case IPv4Protocol::Multiplexing:
            return "Multiplexing";
        case IPv4Protocol::DCN:
            return "DCN";
        case IPv4Protocol::TACMonitoring:
            return "TACMonitoring";
        case IPv4Protocol::AnyLocalNetwork:
            return "AnyLocalNetwork";
        case IPv4Protocol::SATNETAndBackroomEXPAK:
            return "SATNETAndBackroomEXPAK";
        case IPv4Protocol::MITSubnetSupport:
            return "MITSubnetSupport";
        case IPv4Protocol::SATNETMonitoring:
            return "SATNETMonitoring";
        case IPv4Protocol::InternetPacketCoreUtility:
            return "InternetPacketCoreUtility";
        case IPv4Protocol::BackroomSATNETMonitoring:
            return "BackroomSATNETMonitoring";
        case IPv4Protocol::WIDEBANDMonitoring:
            return "WIDEBANDMonitoring";
        case IPv4Protocol::WIDEBANDEXPAK:
            return "WIDEBANDEXPAK";
        default:
            return "None";
    }
}
const char* ipv4header_format2str(IPv4AddressFormat format) {
    switch (format) {
        case IPv4AddressFormat::A:
            return "A";
        case IPv4AddressFormat::B:
            return "B";
        case IPv4AddressFormat::C:
            return "C";
        case IPv4AddressFormat::Extended:
            return "Extended";
        default:
            return "None";
    }
}

void IPv4Header::init() {
    version_length  = 0;
    type_of_service = 0;
    total_length    = 0;
    identification  = 0;
    fragment_offset = 0;
    time_to_live    = 0;
    protocol        = 0;
    checksum        = 0;
    source          = 0;
    destination     = 0;
}

void IPv4Header::set_version(std::uint8_t version) {
    if (version > 0x0Fu)
        return;
    version_length &= 0x0Fu;
    version_length |= static_cast<std::uint8_t>(version << 4);
}

std::uint8_t IPv4Header::get_version() const {
    return static_cast<std::uint8_t>((version_length & 0xF0u) >> 4);
}

/* Header Length */
void IPv4Header::set_header_length(std::uint8_t length) {
    // I don't support option so only 20 bytes
    if (length != 5)
        return;
    version_length &= 0xF0u;
    version_length |= 5u;
}

std::uint8_t IPv4Header::get_header_length() const {
    return version_length & 0x0Fu;
}

/* Type of Service
 *
 * RFC 791 legacy TOS layout:
 *
 *   7   6   5   4   3   2   1   0
 * +-------+---+---+---+-------+
 * |prec.  | D | T | R |  MBZ  |
 * +-------+---+---+---+-------+
 *
 * precedence = bits 7..5
 * delay      = bit 4
 * throughput = bit 3
 * reliability= bit 2
 * MBZ        = bits 1..0
 */
/* Precedence */
void IPv4Header::set_type_of_service_precedence(std::uint8_t precedence) {

    if (precedence > 0x07u)
        return;
    type_of_service &= 0x1Fu;
    type_of_service |= static_cast<std::uint8_t>(precedence << 5);
}
std::uint8_t IPv4Header::get_type_of_service_precedence() const {
    return static_cast<std::uint8_t>(
        (type_of_service >> 5) & 0x07u);
}
/* Delay */
void IPv4Header::set_type_of_service_delay(std::uint8_t delay) {
    if (delay > 1)
        return;
    if (delay)
        type_of_service |= static_cast<std::uint8_t>(1u << 4);
    else
        type_of_service &= static_cast<std::uint8_t>(~(1u << 4));
}
std::uint8_t IPv4Header::get_type_of_service_delay() const {
    return static_cast<std::uint8_t>((type_of_service >> 4) & 0x01u);
}
/* Throughput */
void IPv4Header::set_type_of_service_throughput(std::uint8_t throughput) {
    if (throughput > 1)
        return;
    if (throughput)
        type_of_service |= static_cast<std::uint8_t>(1u << 3);
    else
        type_of_service &= static_cast<std::uint8_t>(~(1u << 3));
}
std::uint8_t IPv4Header::get_type_of_service_throughput() const {
    return static_cast<std::uint8_t>((type_of_service >> 3) & 0x01u);
}
/* Reliability */
void IPv4Header::set_type_of_service_relibility(std::uint8_t relibility) {
    if (relibility > 1)
        return;
    if (relibility)
        type_of_service |= static_cast<std::uint8_t>(1u << 2);
    else
        type_of_service &= static_cast<std::uint8_t>(~(1u << 2));
}
std::uint8_t IPv4Header::get_type_of_service_relibility() const {
    return static_cast<std::uint8_t>((type_of_service >> 2) & 0x01u);
}
/* Total Length */
void IPv4Header::set_total_length(std::uint16_t total_length) {
    if (total_length < 20)
        return;
    this->total_length = total_length;
}
/* Flags */
void IPv4Header::set_df(std::uint8_t df) {
    if (df > 1)
        return;
    if (df)
        fragment_offset |= static_cast<std::uint16_t>(1u << 14);
    else
        fragment_offset &= static_cast<std::uint16_t>(~(1u << 14));
}
void IPv4Header::set_mf(std::uint8_t mf) {
    if (mf > 1)
        return;
    if (mf)
        fragment_offset |= static_cast<std::uint16_t>(1u << 13);
    else
        fragment_offset &= static_cast<std::uint16_t>(~(1u << 13));
}
std::uint16_t IPv4Header::get_df() const {
    return static_cast<std::uint16_t>((fragment_offset >> 14) & 0x01u);
}
std::uint16_t IPv4Header::get_mf() const {
    return static_cast<std::uint16_t>((fragment_offset >> 13) & 0x01u);
}
/* Fragment Offset */
void IPv4Header::set_fragment_offset(std::uint16_t fragment_offset) {
    if (fragment_offset > 0x1FFFu)
        return;
    this->fragment_offset &= 0xE000u;
    this->fragment_offset |= fragment_offset;
}

std::uint16_t IPv4Header::get_fragment_offset() const {
    return fragment_offset & 0x1FFFu;
}

/* Identification */
void IPv4Header::set_identification(std::uint16_t identification) {
    this->identification = identification;
}

/* Time to Live */
void IPv4Header::set_time_to_live(std::uint8_t time_to_live) {
    this->time_to_live = time_to_live;
}
/* Protocol */
void IPv4Header::set_protocol(std::uint8_t protocol) {
    this->protocol = protocol;
}

/* Address
 *
 * Historical classful IPv4 layout:
 *
 * Class A:
 *   0 | network (7 bits) | host (24 bits)
 *
 * Class B:
 *   10 | network (14 bits) | host (16 bits)
 *
 * Class C:
 *   110 | network (21 bits) | host (8 bits)
 *
 * Extended:
 *   111 | remaining bits
 */

/* Address Class */
void IPv4Header::set_address_classful_format(std::uint32_t& address, IPv4AddressFormat format) {
    switch (format) {
        case IPv4AddressFormat::A:
            /* 0xxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx */
            address &= 0x7FFFFFFFu;
            break;

        case IPv4AddressFormat::B:
            /* 10xxxxxx xxxxxxxx xxxxxxxx xxxxxxxx */
            address &= 0x3FFFFFFFu;
            address |= 0x80000000u;
            break;

        case IPv4AddressFormat::C:
            /* 110xxxxx xxxxxxxx xxxxxxxx xxxxxxxx */
            address &= 0x1FFFFFFFu;
            address |= 0xC0000000u;
            break;

        case IPv4AddressFormat::Extended:
            /* 111xxxxx xxxxxxxx xxxxxxxx xxxxxxxx */
            address &= 0x1FFFFFFFu;
            address |= 0xE0000000u;
            break;

        default:
            break;
    }
}

/* Address Network */
void IPv4Header::set_address_classful_network(std::uint32_t& address, IPv4AddressFormat format, std::uint32_t network) {
    std::uint32_t network_mask;
    switch (format) {
        case IPv4AddressFormat::A:
            /*
             * 7-bit network field:
             *
             * 0 NNNNNNN HHHHHHHH HHHHHHHH HHHHHHHH
             */
            network_mask = 0x7F000000u;
            break;

        case IPv4AddressFormat::B:
            /*
             * 14-bit network field:
             *
             * 10 NNNNNNNNNNNNNN HHHHHHHH HHHHHHHH
             */
            network_mask = 0x3FFF0000u;
            break;

        case IPv4AddressFormat::C:
            /*
             * 21-bit network field:
             *
             * 110 NNNNNNNNNNNNNNNNNNNNN HHHHHHHH
             */
            network_mask = 0x00FFFF00u;
            break;

        default:
            return;
    }
    address &= ~network_mask;
    address |= network & network_mask;
}

/* Address Host */
void IPv4Header::set_address_classful_host(std::uint32_t& address, IPv4AddressFormat format, std::uint32_t host) {
    std::uint32_t host_mask;
    switch (format) {
        case IPv4AddressFormat::A:
            /* 24-bit host */
            host_mask = 0x00FFFFFFu;
            break;

        case IPv4AddressFormat::B:
            /* 16-bit host */
            host_mask = 0x0000FFFFu;
            break;

        case IPv4AddressFormat::C:
            /* 8-bit host */
            host_mask = 0x000000FFu;
            break;

        default:
            return;
    }
    address &= ~host_mask;
    address |= host & host_mask;
}

/* Address Network using CIDR prefix */
void IPv4Header::set_address_network(std::uint32_t& address, std::uint32_t network, std::uint8_t prefix) {
    if (prefix > 32)
        return;
    const std::uint32_t network_mask =
        prefix == 0
            ? 0u
            : static_cast<std::uint32_t>(
                  0xFFFFFFFFu << (32u - prefix));
    address &= ~network_mask;
    address |= network & network_mask;
}

/* Address Host using CIDR prefix */
void IPv4Header::set_address_host(std::uint32_t& address, std::uint32_t host, std::uint8_t prefix) {
    if (prefix > 32)
        return;
    const std::uint32_t host_mask =
        prefix == 32
            ? 0u
            : static_cast<std::uint32_t>(
                  0xFFFFFFFFu >> prefix);
    address &= ~host_mask;
    address |= host & host_mask;
}

/* Source Address */
void IPv4Header::set_source_classful(IPv4AddressFormat format, std::uint32_t network, std::uint32_t host) {
    set_address_classful_format(source, format);
    set_address_classful_network(source, format, network);
    set_address_classful_host(source, format, host);
}

void IPv4Header::set_source(std::uint32_t network, std::uint32_t host, std::uint8_t prefix) {
    set_address_network(source, network, prefix);
    set_address_host(source, host, prefix);
}

/* Destination Address */
void IPv4Header::set_destination_classful(IPv4AddressFormat format, std::uint32_t network, std::uint32_t host) {
    set_address_classful_format(destination, format);
    set_address_classful_network(destination, format, network);
    set_address_classful_host(destination, format, host);
}

void IPv4Header::set_destination(std::uint32_t network, std::uint32_t host, std::uint8_t prefix) {
    set_address_network(destination, network, prefix);
    set_address_host(destination, host, prefix);
}

/* Validation */
bool IPv4Header::is_version_valid() const {
    return get_version() == 4;
}

bool IPv4Header::is_header_length_valid() const {
    return get_header_length() == 5;
}

bool IPv4Header::is_fragmentation_offset_valid() const {
    const std::uint16_t df            = get_df();
    const std::uint16_t mf            = get_mf();
    const std::uint16_t offset        = get_fragment_offset();
    const std::uint16_t header_length = static_cast<std::uint16_t>(get_header_length() * 4u);
    if (total_length < header_length)
        return false;
    const std::uint32_t data_length = static_cast<std::uint32_t>(total_length - header_length);
    if (df && offset != 0)
        return false;
    if (df && mf)
        return false;
    if (static_cast<std::uint32_t>(offset) * 8u + data_length > MAX_SIZE_IPV4_PACKET)
        return false;
    return true;
}

bool IPv4Header::is_protocol_valid() const {
    switch (static_cast<IPv4Protocol>(protocol)) {
        case IPv4Protocol::ICMP:
        case IPv4Protocol::GatewayToGateway:
        case IPv4Protocol::CMCCGatewayMonitoringMessage:
        case IPv4Protocol::ST:
        case IPv4Protocol::TCP:
        case IPv4Protocol::UCL:
        case IPv4Protocol::Secure:
        case IPv4Protocol::BBNRCCMonitoring:
        case IPv4Protocol::NVP:
        case IPv4Protocol::PUP:
        case IPv4Protocol::Pluribus:
        case IPv4Protocol::Telenet:
        case IPv4Protocol::XNET:
        case IPv4Protocol::Chaos:
        case IPv4Protocol::UserDatagram:
        case IPv4Protocol::Multiplexing:
        case IPv4Protocol::DCN:
        case IPv4Protocol::TACMonitoring:
        case IPv4Protocol::AnyLocalNetwork:
        case IPv4Protocol::SATNETAndBackroomEXPAK:
        case IPv4Protocol::MITSubnetSupport:
        case IPv4Protocol::SATNETMonitoring:
        case IPv4Protocol::InternetPacketCoreUtility:
        case IPv4Protocol::BackroomSATNETMonitoring:
        case IPv4Protocol::WIDEBANDMonitoring:
        case IPv4Protocol::WIDEBANDEXPAK:
            return true;

        default:
            return false;
    }
}

/* Serialization */
void IPv4Header::build_bytes(std::uint8_t* buffer) const {
    if (buffer == nullptr)
        return;

    std::size_t offset = 0;
    buffer[offset++] = version_length;
    buffer[offset++] = type_of_service;
    unpack_u16_at(buffer, &offset, total_length);
    unpack_u16_at(buffer, &offset, identification);
    unpack_u16_at(buffer, &offset, fragment_offset);
    buffer[offset++] = time_to_live;
    buffer[offset++] = protocol;
    unpack_u16_at(buffer, &offset, checksum);
    unpack_u32_at(buffer, &offset, source);
    unpack_u32_at(buffer, &offset, destination);
}

bool IPv4Header::from_bytes(const std::uint8_t* buffer, std::size_t buffer_size) {
    if (buffer == nullptr)
        return false;
    if (buffer_size < size())
        return false;
    std::size_t offset                         = 0;
    const std::uint8_t parsed_version_length   = buffer[offset++];
    const std::uint8_t parsed_type_of_service  = buffer[offset++];
    const std::uint16_t parsed_total_length    = pack_u16_at(buffer, &offset);
    const std::uint16_t parsed_identification  = pack_u16_at(buffer, &offset);
    const std::uint16_t parsed_fragment_offset = pack_u16_at(buffer, &offset);
    const std::uint8_t parsed_time_to_live     = buffer[offset++];
    const std::uint8_t parsed_protocol         = buffer[offset++];
    const std::uint16_t parsed_checksum        = pack_u16_at(buffer, &offset);
    const std::uint32_t parsed_source          = pack_u32_at(buffer, &offset);
    const std::uint32_t parsed_destination     = pack_u32_at(buffer, &offset);
    version_length                             = parsed_version_length;
    type_of_service                            = parsed_type_of_service;
    total_length                               = parsed_total_length;
    identification                             = parsed_identification;
    fragment_offset                            = parsed_fragment_offset;
    time_to_live                               = parsed_time_to_live;
    protocol                                   = parsed_protocol;
    checksum                                   = parsed_checksum;
    source                                     = parsed_source;
    destination                                = parsed_destination;

    if (!is_version_valid())
        goto invalid;
    if (!is_header_length_valid())
        goto invalid;
    if (total_length < size())
        goto invalid;
    if (total_length > MAX_SIZE_IPV4_PACKET)
        goto invalid;
    if (!is_fragmentation_offset_valid())
        goto invalid;
    if (!is_protocol_valid())
        goto invalid;
    return true;
invalid:
    init();
    return false;
}
std::vector<std::uint8_t> IPv4Header::to_bytes() const {
    std::vector<std::uint8_t> buffer(size());

    build_bytes(buffer.data());

    return buffer;
}
