#ifndef NETSIM_UITLS_HPP
#define NETSIM_UITLS_HPP

#include <chrono>
#include <cstdint>
#include "settings.hpp"

uint64_t get_timestamp_now_ms();
uint64_t get_timestamp_after_ms(uint64_t milliseconds);
uint32_t get_timestamp_now_s();

// Bytes budget management
struct BytesBudget {
    uint64_t budget; // Because it can go pretty high, maybe i should store in MB idk
    uint64_t budget_per_tick;
    uint64_t budget_max_threshold;

    BytesBudget(uint64_t budget_per_tick, uint64_t budget_max_threshold = 10e8); // 1Gb
    void set_bpt_from_bps(uint64_t bytes_per_second);
    void update();
    bool can_consume(uint64_t size);
    bool consume(uint64_t size);
};

#endif
