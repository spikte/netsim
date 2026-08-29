#ifndef GUI_SWITCH_HPP
#define GUI_SWITCH_HPP

#include "../include/switch.hpp"
#include "gui_device.hpp"
#include <QGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>

struct SwitchItem : public DeviceItem {
    SwitchItem(Switch* sw);
    SwitchItem(int nslot);
};

#endif
