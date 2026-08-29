#include "gui_link.hpp"

LinkItem::LinkItem(DeviceItem* src,
                   size_t srcSlot,
                   DeviceItem* dst,
                   size_t dstSlot,
                   Link* link) : src(src),
                                 srcSlot(srcSlot),
                                 dst(dst),
                                 dstSlot(dstSlot),
                                 link(link) {
    setFlag(QGraphicsItem::ItemIsSelectable);
    start = {0, 0};
    end = {0, 0};
    if (src && dst)
        update_geometry();
    if (src)
        src->addLink(this);
    if (dst)
        dst->addLink(this);
}
LinkItem::LinkItem() : src(nullptr), srcSlot(0), dst(nullptr), dstSlot(0), link(nullptr) {
    start = {0, 0};
    end = {0, 0};
}
void LinkItem::set_start(const QPointF& newStart) {
    prepareGeometryChange();
    start = newStart;
    update();
}
void LinkItem::setEnd(const QPointF& new_end) {
    prepareGeometryChange();
    end = new_end;
    update();
}
void LinkItem::update_geometry() {
    prepareGeometryChange();
    QPointF srcPos = src->mapToScene(src->portCenter(srcSlot));
    QPointF dstPos = dst->mapToScene(dst->portCenter(dstSlot));
    setPos(srcPos);
    start = QPointF(0, 0);
    end   = dstPos - srcPos;
    update();
}
QRectF LinkItem::boundingRect() const {
    const qreal margin = width / 2.0;
    return QRectF(start, end).normalized().adjusted(-margin, -margin, margin, margin);
}

void LinkItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QPen pen(Qt::black, width);
    pen.setCapStyle(Qt::RoundCap);

    painter->setPen(pen);
    painter->drawLine(start, end);
}
