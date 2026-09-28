#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "../packet/packet.hpp"
#include "observer.hpp"
#include "settings.hpp"
#include "type.hpp"
#include "utils.hpp"

#include "link.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <queue>
#include <string>
#include <vector>

enum class InterfaceType {
    Ethernet,
    Wireless,
    Serial,
    Loopback
};
enum class PhysicalPortType {
    Coaxial,
    RJ45,
    SFP,
    SFPPlus
};
enum class EthernetPhy {
    // Coaxial
    _10BASE5,
    _10BASE2,
    // Twisted pair
    _10BASE_T,
    _100BASE_TX,
    _1000BASE_T,
    _10GBASE_T,
    // Fiber - multimode
    _10BASE_FL,
    _100BASE_FX,
    _1000BASE_SX,
    _10GBASE_SR,
    // Fiber - single-mode
    _1000BASE_LX,
    _10GBASE_LR
};
enum class MDIType {
    MDI,
    MDIX,
    Auto
};
struct Device;
struct EthernetConfig {
    PhysicalPortType physical_port_type;
    EthernetPhy phy;
    MDIType mdi_type = MDIType::Auto;
};
struct Interface {
    // Identity
    std::string name;
    std::string description;
    Device* device;
    std::size_t device_port;

    // Network configuration
    MACAddress mac_address;
    IPv4CIDR ipv4_cidr;
    std::uint32_t mtu;

    // Physical interface capabilities
    InterfaceType type;
    std::optional<EthernetConfig> ethernet_config;

    // Runtime state
    bool on;

    // Link
    PPLinkUni* egress;
    PPLinkUni* ingress;

    // Queues
    std::priority_queue<ScheduledData, std::vector<ScheduledData>, CompareScheduledData> scheduled_queue;
    std::deque<InTransitData> send_queue;
    std::deque<DataRaw> recv_queue;
    std::size_t send_queued_bytes;
    std::size_t recv_queued_bytes;
    std::size_t buffer_size;

    // Events
    Signal<InTransitData*> on_data_sent;

    // For scheduling
    std::uint8_t data_sequence;

    // Capacities
    std::uint64_t bits_per_second;

    Interface(MACAddress address = 0, EthernetPhy phy = {}, PhysicalPortType physical_port_type = {});
    std::uint32_t max_lenght();
    bool is_compatible(const Interface& interface);
    bool can_send(std::size_t data_size);
    bool can_recv(std::size_t data_size);
    bool is_connectable_ingress();
    bool is_connectable_egress();
    bool schedule(DataRaw data, std::uint64_t scheduled_tick);
    void recv_data(DataRaw data);
    void update();

  private:
    bool send_data(DataRaw data);
};

#endif
