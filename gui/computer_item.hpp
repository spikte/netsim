#ifndef GUI_COMPUTER_ITEM_HPP
#define GUI_COMPUTER_ITEM_HPP

#include "../include/computer.hpp"
#include "device_item.hpp"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include "terminal_widget.hpp"

struct ComputerItem : public DeviceItem {
    TerminalWidget<Computer> terminal;
    ComputerItem(Computer* sw);
    ComputerItem(int nslot);

    QRectF boundingRect() const override;
    QPointF portCenter(size_t port) const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
};

#endif
