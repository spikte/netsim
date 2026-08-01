#ifndef NETSIM_UITLS_HPP
#define NETSIM_UITLS_HPP

#include <chrono>
#include <cstdint>
#include "settings.hpp"

uint64_t get_timestamp_now_ms();
uint64_t get_timestamp_after_ms(uint64_t milliseconds);
uint32_t get_timestamp_now_s();

#endif
