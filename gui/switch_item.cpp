#include "switch_item.hpp"

SwitchItem::SwitchItem(Switch* sw) : DeviceItem(sw, sw->ports.size()) {}
SwitchItem::SwitchItem(int nslot) : DeviceItem(nullptr, nslot) {}
