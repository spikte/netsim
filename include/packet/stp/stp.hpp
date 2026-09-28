#ifndef STP_HPP
#define STP_HPP

#include <cstdint>
#include <vector>
extern "C" {
#include "../utils/numbers.h"
}

enum class BPDUType: std::uint8_t {
    Config = 0x00,
    TCN    = 0x80
};

enum class STPVersion {
    STP,
    RSTP,
    MSTP,
    PVSTPlus,
    RapidPVSTPlus
};

struct BPDU {
    std::uint16_t protocol_id;
    std::uint8_t version_id;
    std::uint8_t bpdu_type;
    enum BPDUFlags : std::uint8_t {
        TopologyChange     = 1u << 0,
        Proposal           = 1u << 1,
        PortRoleAlternate  = 1u << 2,
        PortRoleRoot       = 1u << 3,
        PortRoleDesignated = 1u << 2 | 1u << 3,
        Learning           = 1u << 4,
        Forwarding         = 1u << 5,
        Agreement          = 1u << 6,
        TCA                = 1u << 7
    };
    std::uint8_t flags;
    std::uint64_t root_id;
    std::uint32_t root_path_cost;
    std::uint64_t bridge_id;
    std::uint16_t port_id;
    std::uint16_t message_age;
    std::uint16_t max_age;
    std::uint16_t hello_time;
    std::uint16_t forward_delay;
    std::uint8_t version_1_len;
    std::uint16_t version_3_len;

    std::uint16_t get_bridge_priority() const;
    std::uint64_t get_bridge_address() const;
    std::uint8_t get_port_priority() const;
    std::uint16_t get_port_number() const;
    void set_bridge_priority(std::uint16_t priority);
    void set_bridge_address(std::uint64_t mac_address);
    void set_port_priority(std::uint8_t priority);
    void set_port_number(std::uint16_t port_number);

    std::size_t size() const;
    std::size_t build_bytes(std::uint8_t* buffer) const;
    std::vector<std::uint8_t> to_bytes() const;
    bool from_bytes(const std::uint8_t* buffer, std::size_t buffer_size);
};

#endif
