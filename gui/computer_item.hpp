#ifndef GUI_COMPUTER_ITEM_HPP
#define GUI_COMPUTER_ITEM_HPP

#include "../include/sim/computer.hpp"
#include "assets.hpp"
#include "device_item.hpp"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>

template <typename T>
struct TerminalWidget;
struct ComputerItem : public DeviceItem {
    Computer* computer;
    std::unique_ptr<TerminalWidget<ComputerItem>> terminal;
    QColor vlanVizColor;
    ComputerItem(Computer* sw);
    ComputerItem(int nslot);

    QRectF boundingRect() const override;
    QPointF portCenter(std::size_t port) const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void updateVlanVizColor();
    void setVlanVizEnabled(bool enabled) override;
    bool removeLink(std::size_t port) override;
};

#endif
