#include "link_item.hpp"
#include "network_view.hpp"

/* == PacketDot == */
PacketDot::PacketDot(InTransitPacket* packet) : packet(packet),
                                                color(Qt::red),
                                                radius(5.0),
                                                progress(0.0) {
    tick_duration = (packet->delivery_tick - packet->departure_tick);
    speed         = settings.sim_speed / (tick_duration * TICK_RATE);
}
QRectF PacketDot::boundingRect() const {
    return QRectF(-radius, -radius, 2 * radius, 2 * radius);
}
void PacketDot::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    painter->drawEllipse(boundingRect());
}
void PacketDot::updateProgress(double dt) {
    if (packet && packet->departure_tick > global_tick)
        return;
    speed = settings.sim_speed / (tick_duration * TICK_RATE);
    if (reverse)
        progress -= speed * dt;
    else
        progress += speed * dt;
    if (auto* parent = qgraphicsitem_cast<LinkItem*>(parentItem()))
        setPos(parent->path.pointAtPercent(progress));
}

/* == LinkItem == */
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
    end   = {0, 0};
    if (src && dst)
        updateFromDevices();
    if (src)
        src->addLink(this);
    if (dst)
        dst->addLink(this);
}
LinkItem::LinkItem() : src(nullptr), srcSlot(0), dst(nullptr), dstSlot(0), link(nullptr) {
    start = {0, 0};
    end   = {0, 0};
}
void LinkItem::updatePath() {
    QPointF mid((start.x() + end.x()) / 2.0, (start.y() + end.y()) / 2.0);
    qreal dx   = end.x() - start.x();
    QPointF c1 = start + QPointF(dx * 0.4, 0);
    QPointF c2 = end - QPointF(dx * 0.4, 0);
    path       = QPainterPath(start);
    path.cubicTo(c1, c2, end);
    update();
}
void LinkItem::setStart(const QPointF& newStart) {
    prepareGeometryChange();
    start = newStart;
    updatePath();
}
void LinkItem::setEnd(const QPointF& new_end) {
    prepareGeometryChange();
    end = new_end;
    updatePath();
}
void LinkItem::updateFromDevices() {
    prepareGeometryChange();
    QPointF srcPos = src->mapToScene(src->portCenter(srcSlot));
    QPointF dstPos = dst->mapToScene(dst->portCenter(dstSlot));
    setPos(srcPos);
    start = QPointF(0, 0);
    end   = dstPos - srcPos;
    updatePath();
}
QRectF LinkItem::boundingRect() const {
    const qreal margin = width / 2.0;
    return QRectF(start, end).normalized().adjusted(-margin, -margin, margin, margin);
}
void LinkItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    painter->setRenderHint(QPainter::Antialiasing);

    QColor color = (option->state & QStyle::State_Selected) ? QColor("#4c9aff") : QColor("#5b6270");
    QPen pen(color, width);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);
    painter->drawPath(path);

    // small end dots where it plugs into a port
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    painter->drawEllipse(start, width, width);
    painter->drawEllipse(end, width, width);
}
void LinkItem::sendPacket(InTransitPacket* packet, bool reverse) {
    PacketDot* packetDot;

    packetDot = new PacketDot(packet);
    packetDot->setParentItem(this);
    packetDot->reverse  = reverse;
    packetDot->progress = reverse ? 1 : 0;
    packetDots.push_back(packetDot);
    // If the tick didn't change this frame
    // we make the packet being sent at the next iteration
    if (!hasTickAdvanced && packet->departure_tick == global_tick) {
        packet->departure_tick++;
        packet->delivery_tick++;
    }
}
void LinkItem::updatePacketDots(double dt) {
    for (auto it = packetDots.begin(); it != packetDots.end();) {
        PacketDot* packetDot    = *it;
        InTransitPacket* packet = packetDot->packet;
        if (!packet) {
            delete packetDot;
            it = packetDots.erase(it);
            continue;
        }
        if (packet->delivery_tick <= global_tick) {
            delete packetDot;
            it = packetDots.erase(it);
        } else {
            packetDot->updateProgress(dt);
            it++;
        }
    }
}
