#ifndef IPV4HEADER_HPP
#define IPV4HEADER_HPP

extern "C" {
#include "../utils/numbers.h"
}

#include <cstddef>
#include <cstdint>
#include <vector>

constexpr std::size_t MAX_SIZE_IPV4_PACKET = 65535;

enum class IPv4TOSPrecedence : std::uint8_t {
    NetworkControl      = 7,
    InternetworkControl = 6,
    CRITIC              = 5,
    FlashOverride       = 4,
    Flash               = 3,
    Immediate           = 2,
    Priority            = 1,
    Routine             = 0
};

const char* ipv4header_tosp2str(IPv4TOSPrecedence precedence);

enum class IPv4Protocol : std::uint8_t {
    ICMP                         = 1,
    GatewayToGateway             = 3,
    CMCCGatewayMonitoringMessage = 4,
    ST                           = 5,
    TCP                          = 6,
    UCL                          = 7,
    Secure                       = 9,
    BBNRCCMonitoring             = 10,
    NVP                          = 11,
    PUP                          = 12,
    Pluribus                     = 13,
    Telenet                      = 14,
    XNET                         = 15,
    Chaos                        = 16,
    UserDatagram                 = 17,
    Multiplexing                 = 18,
    DCN                          = 19,
    TACMonitoring                = 20,
    AnyLocalNetwork              = 63,
    SATNETAndBackroomEXPAK       = 64,
    MITSubnetSupport             = 65,
    SATNETMonitoring             = 69,
    InternetPacketCoreUtility    = 71,
    BackroomSATNETMonitoring     = 76,
    WIDEBANDMonitoring           = 78,
    WIDEBANDEXPAK                = 79
};

const char* ipv4header_protocol2str(IPv4Protocol protocol);

enum class IPv4AddressFormat : std::uint8_t {
    A        = 0,
    B        = 2,
    C        = 6,
    Extended = 7
};

const char* ipv4header_format2str(IPv4AddressFormat format);

struct IPv4Header {
    std::uint8_t version_length{};
    std::uint8_t type_of_service{};
    std::uint16_t total_length{};
    std::uint16_t identification{};
    std::uint16_t fragment_offset{};
    std::uint8_t time_to_live{};
    std::uint8_t protocol{};
    std::uint16_t checksum{};
    std::uint32_t source{};
    std::uint32_t destination{};

    void init();

    static constexpr std::size_t size() {
        return 20;
    }

    /* Version */
    void set_version(std::uint8_t version);
    std::uint8_t get_version() const;

    /* Length */
    void set_header_length(std::uint8_t length);
    std::uint8_t get_header_length() const;

    /* Type of service */
    void set_type_of_service_precedence(std::uint8_t type_of_service);
    void set_type_of_service_delay(std::uint8_t delay);
    void set_type_of_service_throughput(std::uint8_t throughput);
    void set_type_of_service_relibility(std::uint8_t relibility);
    std::uint8_t get_type_of_service_precedence() const;
    std::uint8_t get_type_of_service_delay() const;
    std::uint8_t get_type_of_service_throughput() const;
    std::uint8_t get_type_of_service_relibility() const;
    /* Total Length */
    void set_total_length(std::uint16_t total_length);

    /* Identification */
    void set_identification(std::uint16_t identification);

    /* Flags */
    void set_df(std::uint8_t df);
    void set_mf(std::uint8_t mf);
    std::uint16_t get_df() const;
    std::uint16_t get_mf() const;

    /* Fragment Offset */
    void set_fragment_offset(std::uint16_t fragment_offset);
    std::uint16_t get_fragment_offset() const;


    /* Time to Live */
    void set_time_to_live(std::uint8_t time_to_live);

    /* Protocol */
    void set_protocol(std::uint8_t protocol);

    /* Address */
    static void set_address_classful_format(std::uint32_t& address, IPv4AddressFormat format);
    static void set_address_classful_network(std::uint32_t& address, IPv4AddressFormat format, std::uint32_t network);
    static void set_address_classful_host(std::uint32_t& address, IPv4AddressFormat format, std::uint32_t host);
    static void set_address_network(std::uint32_t& address, std::uint32_t network, std::uint8_t prefix);
    static void set_address_host(std::uint32_t& address, std::uint32_t host, std::uint8_t prefix);

    /* Source Address */
    void set_source_classful(IPv4AddressFormat format, std::uint32_t network, std::uint32_t host);
    void set_source(std::uint32_t network, std::uint32_t host, std::uint8_t prefix);

    /* Destination Address */
    void set_destination_classful(IPv4AddressFormat format, std::uint32_t network, std::uint32_t host);
    void set_destination(std::uint32_t network, std::uint32_t host, std::uint8_t prefix);

    bool is_version_valid() const;
    bool is_header_length_valid() const;
    bool is_fragmentation_offset_valid() const;
    bool is_protocol_valid() const;

    void build_bytes(std::uint8_t* buffer) const;
    std::vector<std::uint8_t> to_bytes() const;
    bool from_bytes(const std::uint8_t* buffer, std::size_t buffer_size);
};

#endif
