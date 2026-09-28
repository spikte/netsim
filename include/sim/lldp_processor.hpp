#ifndef LLDP_PROCESSOR_HPP
#define LLDP_PROCESSOR_HPP

#include "device.hpp"

struct LLDPProcessor {
    static void update_egress(Device& device, std::size_t port);
    static void process_frame_lldp(Device& device, std::size_t port, Ethernet& frame);
};

#endif
