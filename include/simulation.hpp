#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "interface.hpp"
#include "switch.hpp"
#include "device.hpp"

struct Simulation {
    std::vector<std::unique_ptr<Switch>> switchs;
    std::vector<std::unique_ptr<Link>> links;

    Switch& add_switch();
    Link& addLink(Device& sw1, size_t slot1, Device& sw2, size_t slot2);
};

#endif
