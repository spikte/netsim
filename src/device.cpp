#include "../include/device.hpp"

bool link_devices(Device& d1, size_t slot1, Device& d2, size_t slot2, Link& link) {
    if (!d1.has_interface(slot1) || !d2.has_interface(slot2))
        return false;
    return link_interface(d1.get_interface(slot1), d2.get_interface(slot2), link);
}
