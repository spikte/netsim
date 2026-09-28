#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "device.hpp"
#include "interface.hpp"
#include "mac_table.hpp"
#include "switching_processor.hpp"
#include "utils.hpp"
#include "vlan_table.hpp"
#include <cassert>
#include <unordered_map>
#include <unordered_set>

/* == Switch == */
struct Switch : Device {
    Switch(std::size_t nport = 8);
    int64_t process_frame(std::size_t port, Ethernet& frame) override;
    void update() override;
};

#endif
