#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "interface.hpp"
#include "switch.hpp"
#include "computer.hpp"
#include "device.hpp"


// TODO: Move some stuff from settings here (actually maybe everything from settings)
struct Simulation {
    std::vector<std::unique_ptr<Switch>> switches;
    std::vector<std::unique_ptr<Computer>> computers;
    std::vector<std::unique_ptr<Link>> links;
    double accumulated_time;

    Simulation();
    Switch& add_switch();
    Computer& add_computer();
    Link& addLink(Device& sw1, size_t slot1, Device& sw2, size_t slot2);
    void set_sim_speed(double sim_speed);
    void update(double dt);
};

#endif
