#ifndef STP_PROCESSOR_HPP
#define STP_PROCESSOR_HPP

#include "device.hpp"

struct STPProcessor {
    static BPDU build_root_bpdu_stp(STPDevice& device, std::size_t port);
    static BPDU build_root_bpdu_rstp(STPDevice& device, std::size_t port);
    static BPDU build_root_bpdu_mstp(STPDevice& device, std::size_t port);
    static BPDU build_root_bpdu_pvstplus(STPDevice& device, std::size_t port);
    static BPDU build_root_bpdu_rapid_pvstplus(STPDevice& device, std::size_t port);

    static BPDU forward_bpdu_stp(STPDevice& device, std::size_t port, BPDU bpdu);

    static void update_egress(STPDevice& device, std::size_t port);
    static void process_frame_stp(Device& device, std::size_t port, Ethernet& frame);
};

#endif
