#include "../include/settings.hpp"

Settings settings;
void init_settings() {
    settings.sim_speed = 1.0;
    settings.mac_aging_default = (5 * 60) / TICK_RATE;
}
