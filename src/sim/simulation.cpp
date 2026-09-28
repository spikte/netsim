#include "../../include/sim/simulation.hpp"

Simulation::Simulation() : accumulated_time(0) {
    init_settings();
}
Simulation::Simulation(const std::string& path) : accumulated_time(0) {
    init_settings();
}
Router& Simulation::add_router() {
    routers.push_back(std::make_unique<Router>());
    Router& router = *routers.back();
    return router;
}
Switch& Simulation::add_switch() {
    switches.push_back(std::make_unique<Switch>());
    Switch& sw = *switches.back();
    return sw;
}
Computer& Simulation::add_computer() {
    computers.push_back(std::make_unique<Computer>());
    Computer& computer = *computers.back();
    return computer;
}
Link* Simulation::add_link(Device& device1, std::size_t slot1, Device& device2, std::size_t slot2) {
    links.push_back(std::make_unique<Link>());
    Link& link         = *links.back();
    if(!link.connect(device1, slot1, device2, slot2)) {
        links.pop_back();
        return nullptr;
    }
    return &link;
}

void Simulation::delete_router(Router* router) {
    auto it = std::find_if(routers.begin(), routers.end(), [router](const std::unique_ptr<Router>& el) {
        return router == el.get();
    });
    if (it != routers.end()) {
        delete_link(it->get());
        routers.erase(it);
    }
}
void Simulation::delete_switch(Switch* sw) {
    auto it = std::find_if(switches.begin(), switches.end(), [sw](const std::unique_ptr<Switch>& el) {
        return sw == el.get();
    });
    if (it != switches.end()) {
        delete_link(it->get());
        switches.erase(it);
    }
}
void Simulation::delete_computer(Computer* computer) {
    auto it = std::find_if(computers.begin(), computers.end(), [computer](const std::unique_ptr<Computer>& el) {
        return computer == el.get();
    });
    if (it != computers.end()) {
        delete_link(it->get());
        computers.erase(it);
    }
}
void Simulation::delete_link(Device* device) {
    for (auto* link : device->links)
        delete_link(link);
}
void Simulation::delete_link(Link* link) {
    auto it = std::find_if(links.begin(), links.end(), [link](const std::unique_ptr<Link>& el) {
        return link == el.get();
    });
    if (it != links.end()) {
        Link* link = it->get();
        links.erase(it);
    }
}
void Simulation::set_sim_speed(double sim_speed) {
    settings.sim_speed = sim_speed;
}
void Simulation::update(double dt) {
    accumulated_time += dt * settings.sim_speed;
    if (accumulated_time >= TICK_DURATION) {
        std::uint64_t tick_passed = static_cast<std::uint64_t>(accumulated_time / TICK_DURATION);
        global_tick += tick_passed;
        accumulated_time -= tick_passed * TICK_DURATION;
        for (auto& router : routers)
            router->update_interfaces_egress();
        for (auto& sw : switches)
            sw->update_interfaces_egress();
        for (auto& computer : computers)
            computer->update_interfaces_egress();
        for (auto& router : routers)
            router->update();
        for (auto& sw : switches)
            sw->update();
        for (auto& computer : computers)
            computer->update();
    }
}
