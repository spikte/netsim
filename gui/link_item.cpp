#include "link_item.hpp"
#include "network_view.hpp"

/* == PacketDot == */
PacketDot::PacketDot(LinkItem* parent, InTransitData* packet) : QGraphicsObject(parent), packet(packet),
                                                                color(Qt::red),
                                                                radius(5.0),
                                                                progress(0.0),
                                                                inspector() {
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    tick_duration = (packet->delivery_tick - packet->departure_tick);
    speed         = settings.sim_speed / (tick_duration * TICK_DURATION);
    inspector     = new Inspector(nullptr, packet->data);
    if (auto* view = parent->getNetworkView())
        inspector->setParent(view);
    inspector->hide();
}
PacketDot::~PacketDot() {
    delete inspector;
}
QRectF PacketDot::boundingRect() const {
    return QRectF(-radius, -radius, 2 * radius, 2 * radius);
}
void PacketDot::paint(QPainter* painter,
                      const QStyleOptionGraphicsItem* option,
                      QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    if (isSelected()) {
        qreal outlineRadius = radius * 1.2;
        QRectF outlineRect(-outlineRadius, -outlineRadius, 2 * outlineRadius, 2 * outlineRadius);
        painter->setBrush(QColor("#4c9aff"));
        painter->drawEllipse(outlineRect);
    }
    painter->setBrush(QColor("#FF9800"));
    painter->drawEllipse(boundingRect());
}
void PacketDot::updateProgress(double dt) {
    if (packet && packet->departure_tick > global_tick)
        return;
    speed = settings.sim_speed / (tick_duration * TICK_DURATION);
    if (reverse) {
        progress -= speed * dt;
        if (progress < 0)
            progress = 0;
    } else {
        progress += speed * dt;
        if (progress > 1)
            progress = 1;
    }
    if (auto* parent = qgraphicsitem_cast<LinkItem*>(parentItem()))
        setPos(parent->path.pointAtPercent(progress));
}
QVariant PacketDot::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == QGraphicsItem::ItemSelectedHasChanged) {
        const bool selected = value.toBool();
        if (selected)
            inspector->show();
        else
            inspector->hide();
    }
    return QGraphicsObject::itemChange(change, value);
}

/* == LinkItem == */
LinkItem::LinkItem(DeviceItem* src, std::size_t srcSlot, DeviceItem* dst, std::size_t dstSlot, Link* link, QGraphicsItem* parent) : src(src),
                                                                                                                                    srcSlot(srcSlot),
                                                                                                                                    dst(dst),
                                                                                                                                    dstSlot(dstSlot),
                                                                                                                                    link(link),
                                                                                                                                    QGraphicsItem(parent) {
    setFlag(QGraphicsItem::ItemIsSelectable);
    start = {0, 0};
    end   = {0, 0};
    if (src && dst)
        updateFromDevices();
    if (src)
        src->addLink(srcSlot, this);
    if (dst)
        dst->addLink(dstSlot, this);
    Interface* intSrc  = src->device->get_interface(srcSlot);
    Interface* intDst  = dst->device->get_interface(dstSlot);
    onDataSentSrcToken = intSrc->on_data_sent.subscribe([this](InTransitData* packet) { sendPacket(packet, false); });
    onDataSentDstToken = intDst->on_data_sent.subscribe([this](InTransitData* packet) { sendPacket(packet, true); });
}
LinkItem::LinkItem(QGraphicsItem* parent) : src(nullptr), srcSlot(0), dst(nullptr), dstSlot(0), link(nullptr), QGraphicsItem(parent) {
    start = {0, 0};
    end   = {0, 0};
}
LinkItem::~LinkItem() {
    if (src) {
        src->removeLink(this);
        if (src->device && src->device->has_interface(srcSlot)) {
            Interface* intSrc = src->device->get_interface(srcSlot);
            intSrc->on_data_sent.unsubscribe(onDataSentSrcToken);
        }
    }
    if (dst) {
        dst->removeLink(this);
        if (dst->device && dst->device->has_interface(dstSlot)) {
            Interface* intDst = dst->device->get_interface(dstSlot);
            intDst->on_data_sent.unsubscribe(onDataSentDstToken);
        }
    }
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
QPainterPath LinkItem::shape() const {
    QPainterPathStroker stroker;
    stroker.setWidth(width + 6.0);
    stroker.setCapStyle(Qt::RoundCap);

    return stroker.createStroke(path);
}
QRectF LinkItem::boundingRect() const {
    const qreal margin = width / 2.0;
    return QRectF(start, end).normalized().adjusted(-margin, -margin, margin, margin);
}
void LinkItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    painter->setRenderHint(QPainter::Antialiasing);

    QColor color = (option->state & QStyle::State_Selected) ? QColor("#4c9aff") : QColor("#FF9800");
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
void LinkItem::sendPacket(InTransitData* packet, bool reverse) {
    PacketDot* packetDot;
    packetDot           = new PacketDot(this, packet);
    packetDot->reverse  = reverse;
    packetDot->progress = reverse ? 1 : 0;
    packetDot->setPos(path.pointAtPercent(packetDot->progress));
    packetDots.push_back(packetDot);
}
void LinkItem::updatePacketDots(double dt) {
    for (auto it = packetDots.begin(); it != packetDots.end();) {
        PacketDot* packetDot  = *it;
        InTransitData* packet = packetDot->packet;
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
NetworkView* LinkItem::getNetworkView() {
    NetworkView* view;

    view = nullptr;
    if (scene()) {
        const QList<QGraphicsView*> views = scene()->views();
        if (!views.isEmpty())
            view = dynamic_cast<NetworkView*>(views.first());
    }

    return view;
}
void LinkItem::setFilter(EthType eth) {
    ethTypeFilter = eth;
}
void LinkItem::setFilter(IPv4Protocol ipv4_protocol) {
    ipv4ProtocolFilter = ipv4_protocol;
}
void LinkItem::applyFilter() {
    if (ethTypeFilter)
        for (QGraphicsItem* child : childItems()) {
            if (PacketDot* packetDot = dynamic_cast<PacketDot*>(child)) {
                if (ipv4ProtocolFilter.has_value()) {
                    Ethernet frame;
                    if (!frame.from_bytes(packetDot->packet->data.data(), packetDot->packet->data.size())) {
                        packetDot->hide();
                        continue;
                    }
                    IPv4 packet;
                    if (!packet.from_bytes(frame.payload.data(), frame.payload.size())) {
                        packetDot->hide();
                        continue;
                    }
                    if (static_cast<IPv4Protocol>(packet.header.protocol) != *ipv4ProtocolFilter)
                        packetDot->hide();
                    else
                        packetDot->show();
                } else if (ethTypeFilter.has_value()) {
                    Ethernet frame;
                    if (!frame.from_bytes(packetDot->packet->data.data(), packetDot->packet->data.size())) {
                        packetDot->hide();
                        continue;
                    }
                    if (static_cast<EthType>(frame.header.eth_type) != *ethTypeFilter)
                        packetDot->hide();
                    else
                        packetDot->show();
                } else
                    packetDot->show();
            }
        }
}
