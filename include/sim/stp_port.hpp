#ifndef STP_PORT_HPP
#define STP_PORT_HPP

#include "interface.hpp"
#include "utils.hpp"
#include <cstdint>

/* STPPort */
enum class STPPortState {
    Blocking,
    Listening,
    Learning,
    Forwarding,
    Disabled
};
enum class STPPortRole {
    Root,
    Designated,
    NonDesignated,
    Alternate,
    Backup
};
struct STPDevice;
struct STPPort {
    Interface* interface;
    bool enabled       = true;
    STPPortState state = STPPortState::Blocking;
    STPPortRole role   = STPPortRole::Designated;

    std::uint8_t port_priority = 8;
    std::uint64_t last_send;
    std::uint32_t cost;

    STPPort(Interface* interface);
    void enable();
    void disable();
    std::uint16_t port_id();
    void set_cost_short();
    void set_cost_long();

    std::optional<std::uint64_t> received_root_id;
    std::optional<std::uint32_t> received_root_cost;
    std::optional<std::uint64_t> received_bridge_id;
    std::optional<std::uint16_t> received_port_id;
};

/* STPDevice */
struct STPDevice {
    Device* device;
    std::uint64_t root_id;
    std::uint32_t root_cost     = 0;
    std::uint16_t priority      = 32'768;
    std::uint64_t hello_time    = second_to_tick(2);
    std::uint64_t max_age       = second_to_tick(20);
    std::uint64_t forward_delay = second_to_tick(15);
    STPVersion version          = STPVersion::STP;
    std::vector<STPPort> ports;

    STPDevice(Device* device);
    std::uint64_t bridge_id();
    void set_cost();
    void update_root(std::size_t port);
};

#endif
