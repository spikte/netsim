#ifndef NETSIM_SETTINGS_HPP
#define NETSIM_SETTINGS_HPP

constexpr double TICK_RATE = 0.01; // In seconds

struct Settings {
    double sim_speed;
    int mac_aging_default;
};
extern Settings settings;

void init_settings();

#endif
