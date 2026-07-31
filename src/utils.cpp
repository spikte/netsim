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
BytesBudget::BytesBudget(uint64_t budget_per_tick, uint64_t budget_max_threshold) : budget_per_tick(budget_per_tick),
                                                                                    budget_max_threshold(budget_max_threshold),
                                                                                    budget(0) {}

void BytesBudget::set_bpt_from_bps(uint64_t bytes_per_second) {
    budget_per_tick = bytes_per_second * TICK_RATE;
}
void BytesBudget::update() {
    if (budget < budget_max_threshold)
        budget += budget_per_tick;
}
bool BytesBudget::can_consume(uint64_t size) {
    if (budget < size)
        return false;
    return true;
}
bool BytesBudget::consume(uint64_t size) {
    if (!can_consume(size))
        return false;
    budget -= size;
    return true;
}
