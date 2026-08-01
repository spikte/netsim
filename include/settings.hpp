#ifndef NETSIM_SETTINGS_HPP
#define NETSIM_SETTINGS_HPP

#include <cstdint>

constexpr double TICK_RATE = 0.01; // In seconds
extern uint64_t global_tick;

struct Settings {
    double sim_speed;
    int mac_aging_default;
};
extern Settings settings;

void init_settings();

#endif
