#ifndef GUI_LINK_HPP
#define GUI_LINK_HPP

#include <QGraphicsItem>
#include <QPainter>

#include "../include/interface.hpp"
#include "gui_device.hpp"

struct LinkItem : public QGraphicsItem {
  public:
    DeviceItem* src;
    size_t srcSlot;
    DeviceItem* dst;
    size_t dstSlot;
    Link* link;

    QPointF start;
    QPointF end;
    qreal width = 3.0;
    LinkItem(DeviceItem* src,
             size_t srcSlot,
             DeviceItem* dst,
             size_t dstSlot,
             Link* link);
    LinkItem();
    void set_start(const QPointF& newStart);
    void setEnd(const QPointF& newEnd);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void update_geometry();
};

#endif
