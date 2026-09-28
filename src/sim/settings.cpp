#include "../../include/sim/settings.hpp"

Settings settings;
std::uint64_t global_tick = 0;
void init_settings() {
    settings.sim_speed         = 1.0;
    settings.mac_aging_default = second_to_tick(300);
}

std::uint64_t second_to_tick(double seconds) {
    return static_cast<std::uint64_t>(seconds / TICK_DURATION);
}
double tick_to_second(std::uint64_t tick) {
    return tick * TICK_DURATION;
}
double tick_to_ms(std::uint64_t tick) {
    return tick * TICK_DURATION * 1000;
}
