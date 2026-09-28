#ifndef GUI_LINK_ITEM_HPP
#define GUI_LINK_ITEM_HPP

#include "../include/sim/interface.hpp"
#include "inspector.hpp"
#include "device_item.hpp"
#include "settings.hpp"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <algorithm>

struct LinkItem;
struct PacketDot : public QGraphicsObject {
    Q_OBJECT
  public:
    InTransitData* packet;
    QColor color;
    qreal radius;
    qreal progress;
    std::uint64_t tick_duration;
    qreal speed;
    bool reverse;
    Inspector* inspector;

    PacketDot(LinkItem* parent, InTransitData* packet);
    ~PacketDot();
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void updateProgress(double dt);
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
};

struct LinkItem : public QGraphicsItem {
  public:
    DeviceItem* src;
    std::size_t srcSlot;
    DeviceItem* dst;
    std::size_t dstSlot;
    Link* link;
    QPainterPath path;
    std::vector<PacketDot*> packetDots;
    std::size_t onDataSentSrcToken;
    std::size_t onDataSentDstToken;
    std::optional<EthType> ethTypeFilter;
    std::optional<IPv4Protocol> ipv4ProtocolFilter;

    QPointF start;
    QPointF end;
    qreal width = 2.0;
    LinkItem(DeviceItem* src,
             std::size_t srcSlot,
             DeviceItem* dst,
             std::size_t dstSlot,
             Link* link,
             QGraphicsItem* parent = nullptr);
    LinkItem(QGraphicsItem* parent = nullptr);
    ~LinkItem();
    void setStart(const QPointF& newStart);
    void setEnd(const QPointF& newEnd);
    QPainterPath shape() const override;
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void updatePath();
    void updateFromDevices();
    void sendPacket(InTransitData* packet, bool reverse);
    void updatePacketDots(double dt);
    NetworkView* getNetworkView();
    void setFilter(EthType eth);
    void setFilter(IPv4Protocol ipv4_protocol);
    void applyFilter();
};


#endif
