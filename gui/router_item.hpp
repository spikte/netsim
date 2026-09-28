#ifndef GUI_ROUTER_ITEM_HPP
#define GUI_ROUTER_ITEM_HPP

#include "../include/sim/router.hpp"
#include "device_item.hpp"

template <typename T>
struct TerminalWidget;
struct RouterItem: public DeviceItem {
    Router* router;
    std::unique_ptr<TerminalWidget<RouterItem>> terminal;
    RouterItem(Router* router);
    RouterItem(int nslot);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QRectF boundingRect() const override;
};

#endif
