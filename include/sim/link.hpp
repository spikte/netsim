#ifndef LINK_HPP
#define LINK_HPP

#include "../packet/packet.hpp"
#include "type.hpp"

struct Interface;
struct Device;
struct ScheduledData {
    DataRaw data;
    std::uint64_t scheduled_tick;
    std::uint64_t sequence;
};
struct CompareScheduledData {
    bool operator()(const ScheduledData& a, const ScheduledData& b) const;
};
struct InTransitData {
    DataRaw data;
    std::uint64_t departure_tick;
    std::uint64_t delivery_tick;
};

DataRaw ethernet_to_data_raw(const Ethernet* ethernet);
DataRaw ipv4_to_data_raw(const IPv4* ipv4);

std::uint64_t bits_to_bytes(std::uint64_t value);
struct PPLinkUni {
    std::uint64_t propagation_speed; // Meters/Seconds
    std::uint64_t length;            // In meters
    Interface* src;
    Interface* dst;

    PPLinkUni(std::uint64_t propagation_speed = 0);
    Device* get_device_src();
    Device* get_device_dst();
    int64_t get_device_port_src();
    int64_t get_device_port_dst();
    bool connect_src(Device& device, std::size_t port);
    bool connect_dst(Device& device, std::size_t port);
    bool connect(Device& device_src, std::size_t port_src, Device& device_dst, std::size_t port_dst);
    Interface* disconnect_src();
    Interface* disconnect_dst();
    void disconnect();
};

enum class MediumType {
    TwistedPair,
    Coaxial,
    Fiber,
    Wireless,
    Serial
};
// Copper
struct TwistedPairProperty {
    enum class Category {
        Cat5e,
        Cat6,
        Cat6a
    };
    enum class Wiring {
        StraightThrough,
        Crossover,
    };
    enum class Shielding {
        UTP,
        STP
    };
    Category category;
    Wiring wiring;
    Shielding shielding;
};
// Coaxial
struct CoaxialProperty {
};
// Fiber
struct FiberProperty {
    enum class Mode {
        Multimode,
        SingleMode
    };
    Mode mode;
};

struct Link {
    std::array<PPLinkUni, 2> pplinks;
    MediumType medium;
    std::optional<TwistedPairProperty> twisted_pair_property;
    std::optional<CoaxialProperty> coaxial_property;
    std::optional<FiberProperty> fiber_property;

    Link(TwistedPairProperty property = {});
    Link(CoaxialProperty property);
    Link(FiberProperty property);
    Link(const Link&)            = delete;
    Link& operator=(const Link&) = delete;
    ~Link();
    std::uint32_t max_length() const;
    Interface* get_interface_src(std::size_t index);
    Interface* get_interface_dst(std::size_t index);
    Device* get_device_src(std::size_t index);
    Device* get_device_dst(std::size_t index);
    int64_t get_device_port_src(std::size_t index);
    int64_t get_device_port_dst(std::size_t index);
    void set_speed();

    bool connect(Device& device1, std::size_t port1, Device& device2, std::size_t port2);
    void disconnect(std::size_t index);
    void disconnect();
};

#endif
