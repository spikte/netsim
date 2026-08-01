#include "../include/utils.hpp"

uint64_t get_timestamp_now_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}
uint64_t get_timestamp_after_ms(uint64_t milliseconds) {
    return get_timestamp_now_ms() + milliseconds;
}
uint32_t get_timestamp_now_s() {
    return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}
