#ifndef ICMPHEADER_HPP
#define ICMPHEADER_HPP

extern "C" {
#include "../utils/numbers.h"
}
#include <cstddef>
#include <cstdint>
#include <vector>

enum class ICMPType : std::uint8_t {
    EchoReply                 = 0,
    DestinationUnreachable    = 3,
    SourceQuench              = 4, // Deprecated
    Redirect                  = 5,
    EchoRequest               = 8,
    RouterAdvertisement       = 9,
    RouterSolicitation        = 10,
    TimeExceeded              = 11,
    ParameterProblem          = 12,
    TimestampRequest          = 13,
    TimestampReply            = 14,
    InformationRequest        = 15, // Deprecated
    InformationReply          = 16, // Deprecated
    AddressMaskRequest        = 17,
    AddressMaskReply          = 18,
    Traceroute                = 30,
    DatagramConversionError   = 31,
    MobileHostRedirect        = 32,
    IPv6WhereAreYou           = 33,
    IPv6IAmHere               = 34,
    MobileRegistrationRequest = 35,
    MobileRegistrationReply   = 36,
    DomainNameRequest         = 37,
    DomainNameReply           = 38,
    SKIP                      = 39,
    Photuris                  = 40,
    ExtendedEchoRequest       = 42,
    ExtendedEchoReply         = 43
};

enum class ICMPCode : std::uint8_t {
    // General
    NetworkUnreachable  = 0,
    HostUnreachable     = 1,
    ProtocolUnreachable = 2,
    PortUnreachable     = 3,
    FragmentationNeeded = 4,
    SourceRouteFailed   = 5,
    // Redirect
    RedirectNetwork    = 0,
    RedirectHost       = 1,
    RedirectNetworkTOS = 2,
    RedirectHostTOS    = 3,
    // Echo, Timestamp, etc.
    NoCode = 0,
    // Time Exceeded
    TTLExceeded                = 0,
    FragmentReassemblyExceeded = 1,
    // Parameter Problem
    PointerIndicatesError = 0,
    MissingRequiredOption = 1,
    BadLength             = 2,
    // Router Discovery
    NormalRouterDiscovery = 0,
    // Extended Echo
    NoError                   = 0,
    MalformedQuery            = 1,
    NoSuchInterface           = 2,
    NoSuchTableEntry          = 3,
    MultipleInterfacesSatisfy = 4
};

struct ICMPHeader {
    std::uint8_t type;
    std::uint8_t code;
    std::uint16_t checksum;
    std::uint32_t rest;

    void init();
    std::size_t size() const;

    void set_type(ICMPType type);
    void set_code(ICMPCode code);
    void set_checksum(std::uint16_t checksum);
    void set_rest(std::uint32_t rest);
    void set_rest_byte(std::uint8_t byte, std::size_t index);
    void set_rest_word(std::uint16_t word, std::size_t index);
    std::uint8_t get_rest_byte(std::size_t index) const;
    std::uint16_t get_rest_word(std::size_t index) const;

    void build_bytes(std::uint8_t* buffer) const;
    std::vector<std::uint8_t> to_bytes() const;
    bool from_bytes(const std::uint8_t* buffer, std::size_t buffer_size);
};

#endif
