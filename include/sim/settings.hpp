#ifndef NETSIM_SETTINGS_HPP
#define NETSIM_SETTINGS_HPP

#include <cstdint>

constexpr double TICK_DURATION = 0.01; // In seconds
extern std::uint64_t global_tick;

struct Settings {
    double sim_speed;
    int mac_aging_default;
};
extern Settings settings;

void init_settings();
std::uint64_t second_to_tick(double seconds);
double tick_to_second(std::uint64_t tick);
double tick_to_ms(std::uint64_t tick);

#endif
