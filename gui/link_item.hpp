#ifndef GUI_LINK_ITEM_HPP
#define GUI_LINK_ITEM_HPP

#include "../include/interface.hpp"
#include "device_item.hpp"
#include "settings.hpp"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <algorithm>

struct PacketDot : public QGraphicsItem {
  public:
    InTransitPacket* packet;
    QColor color;
    qreal radius;
    qreal progress;
    uint64_t tick_duration;
    qreal speed;
    bool reverse;

    PacketDot(InTransitPacket* packet);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void updateProgress(double dt);
};

struct LinkItem : public QGraphicsItem {
  public:
    DeviceItem* src;
    size_t srcSlot;
    DeviceItem* dst;
    size_t dstSlot;
    Link* link;
    QPainterPath path;
    std::vector<PacketDot*> packetDots;

    QPointF start;
    QPointF end;
    qreal width = 2.0;
    LinkItem(DeviceItem* src,
             size_t srcSlot,
             DeviceItem* dst,
             size_t dstSlot,
             Link* link);
    LinkItem();
    void setStart(const QPointF& newStart);
    void setEnd(const QPointF& newEnd);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void updatePath();
    void updateFromDevices();
    void sendPacket(InTransitPacket* packet, bool reverse);
    void updatePacketDots(double dt);
};

#endif
