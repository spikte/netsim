#ifndef GUI_SWITCH_ITEM_HPP
#define GUI_SWITCH_ITEM_HPP

#include "../include/switch.hpp"
#include "device_item.hpp"
#include <QGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>

struct SwitchItem : public DeviceItem {
    SwitchItem(Switch* sw);
    SwitchItem(int nslot);
};

#endif
