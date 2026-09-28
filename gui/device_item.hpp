#ifndef GUI_DEVICE_ITEM_HPP
#define GUI_DEVICE_ITEM_HPP

#include "../include/sim/device.hpp"
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
    bool hovered;
    int hoveredPort;
    bool portHighlightEnabled;
    bool vlanVizEnabled;
    std::vector<QColor> vlanVizPortColors;
    std::vector<LinkItem*> links;

    DeviceItem(Device* device, int nport);
    void setPortHighlightEnabled(bool enabled);
    virtual void setVlanVizEnabled(bool enabled);
    virtual QPointF portCenter(std::size_t port) const;
    void addLink(std::size_t port, LinkItem* link);
    bool removeLink(LinkItem* link);
    virtual bool removeLink(std::size_t port);
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    NetworkView* getNetworkView();
    QPainterPath portCollisionShape() const;
};

#endif
