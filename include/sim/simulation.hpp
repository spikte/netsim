#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "interface.hpp"
#include "router.hpp"
#include "switch.hpp"
#include "computer.hpp"
#include "device.hpp"
#include <vector>
#include <algorithm>


// TODO: Move some stuff from settings here (actually maybe everything from settings)
struct Simulation {
    std::vector<std::unique_ptr<Router>> routers;
    std::vector<std::unique_ptr<Switch>> switches;
    std::vector<std::unique_ptr<Computer>> computers;
    std::vector<std::unique_ptr<Link>> links;
    std::vector<PPLinkUni*> pp_uni_links;
    double accumulated_time;

    Simulation();
    Simulation(const std::string& path);

    Router& add_router();
    Switch& add_switch();
    Computer& add_computer();
    Link* add_link(Device& device1, std::size_t slot1, Device& device2, std::size_t slot2);

    void delete_router(Router* router);
    void delete_switch(Switch* sw);
    void delete_computer(Computer* computer);
    void delete_link(Device* device);
    void delete_link(Link* link);

    void set_sim_speed(double sim_speed);
    void update(double dt);
};

#endif
