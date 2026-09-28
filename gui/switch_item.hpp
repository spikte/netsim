#ifndef GUI_SWITCH_ITEM_HPP
#define GUI_SWITCH_ITEM_HPP

#include "../include/sim/switch.hpp"
#include "../include/sim/computer.hpp"
#include "gui_utils.hpp"
#include "device_item.hpp"
//#include "terminal_widget.hpp"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>

template <typename T>
struct TerminalWidget;
struct SwitchItem : public DeviceItem {
    Switch* sw;
    std::unique_ptr<TerminalWidget<SwitchItem>> terminal;
    SwitchItem(Switch* sw);
    SwitchItem(int nslot);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void updateVlanVizPort(std::size_t port);
    void setVlanVizEnabled(bool enabled) override;
    bool portSetType(std::size_t port, SwitchPortType type);
    bool portSetType(const std::string& name, SwitchPortType type);
    bool portAddVlan(std::size_t port, std::uint16_t vlan);
    bool portAddVlan(const std::string& name, std::uint16_t vlan);
    bool portRmVlan(std::size_t port, std::uint16_t vlan);
    bool portRmVlan(const std::string& name, std::uint16_t vlan);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QPainterPath shape() const override;
    QRectF boundingRect() const override;
    QPainterPath portCollisionShape() const;
};

#endif
