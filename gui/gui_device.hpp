#ifndef GUI_DEVICE_HPP
#define GUI_DEVICE_HPP

#include <QGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <algorithm>
#include "../include/device.hpp"
#include "settings.hpp"

struct LinkItem;
struct DeviceItem : public QGraphicsItem {
    Device* device;
    int nslot;
    int hoveredPort;
    bool portHighlightEnabled;
    std::vector<LinkItem*> links;

    DeviceItem(Device* device, int nslot);
    void setPortHighlightEnabled(bool enabled);
    QPointF portCenter(size_t slot) const;
    void addLink(LinkItem* link);
    void removeLink(LinkItem* link);
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
};

#endif
