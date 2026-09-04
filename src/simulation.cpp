#include "../include/simulation.hpp"

Simulation::Simulation() {
    init_settings();
}
Switch& Simulation::add_switch() {
    switches.push_back(std::make_unique<Switch>());
    return *switches.back();
}
Computer& Simulation::add_computer() {
    computers.push_back(std::make_unique<Computer>());
    return *computers.back();
}
Link& Simulation::addLink(Device& sw1, size_t slot1, Device& sw2, size_t slot2) {
    links.push_back(std::make_unique<Link>());
    Link& link = *links.back();
    link_devices(sw1, slot1, sw2, slot2, link);
    return link;
}
void Simulation::set_sim_speed(double sim_speed) {
    settings.sim_speed = sim_speed;
}
void Simulation::update(double dt) {
    accumulated_time += dt * settings.sim_speed;
    if(accumulated_time >= TICK_RATE) {
        uint64_t tick_passed = static_cast<uint64_t>(accumulated_time / TICK_RATE);
        global_tick += tick_passed;
        accumulated_time -= tick_passed * TICK_RATE;
    }
    for(auto& sw: switches)
        sw->update();
    for(auto& computer: computers)
        computer->update();
}
