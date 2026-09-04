#ifndef GUI_DEVICE_ITEM_HPP
#define GUI_DEVICE_ITEM_HPP

#include "../include/device.hpp"
#include "dialog.hpp"
#include "settings.hpp"
#include <QGraphicsItem>
#include <QMenu>
#include <QStyle>
#include <algorithm>

struct LinkItem;
struct NetworkView;

struct DeviceItem : public QGraphicsItem {
    Device* device;
    int nport;
    int hoveredPort;
    bool portHighlightEnabled;
    std::vector<LinkItem*> links;

    DeviceItem(Device* device, int nport);
    void setPortHighlightEnabled(bool enabled);
    virtual QPointF portCenter(size_t port) const;
    void addLink(LinkItem* link);
    void removeLink(LinkItem* link);
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
};

#endif
