#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <string>
#include "interface.hpp"

struct Device {
    std::string name;
    virtual ~Device() = default;

    virtual size_t interface_count() const = 0;
    virtual bool has_interface(size_t slot) const = 0;
    virtual Interface&get_interface(size_t slot) = 0;
    virtual std::string interface_name(size_t slot) const = 0;
    virtual std::optional<size_t> get_slot_by_name(const std::string& name) const = 0;
};
bool link_devices(Device& d1, size_t slot1, Device& d2, size_t slot2, Link& link);

#endif
