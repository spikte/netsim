#include "gui_switch.hpp"

SwitchItem::SwitchItem(Switch* sw) : DeviceItem(sw, sw->ports.size()) {}
SwitchItem::SwitchItem(int nslot) : DeviceItem(nullptr, nslot) {}
