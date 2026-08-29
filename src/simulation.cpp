#include "../include/simulation.hpp"

Switch& Simulation::add_switch() {
    switchs.push_back(std::make_unique<Switch>());
    return *switchs.front();
}
Link& Simulation::addLink(Device& sw1, size_t slot1, Device& sw2, size_t slot2) {
    links.push_back(std::make_unique<Link>());
    Link& link = *links.front();
    link_devices(sw1, slot1, sw2, slot2, link);
    return link;
}
