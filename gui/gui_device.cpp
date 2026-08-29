#include "gui_device.hpp"
#include "gui_link.hpp"

DeviceItem::DeviceItem(Device* device, int nslot) : device(device), nslot(nslot), hoveredPort(-1), portHighlightEnabled(false), QGraphicsItem() {
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}
void DeviceItem::setPortHighlightEnabled(bool enabled) {
    portHighlightEnabled = enabled;
}
QPointF DeviceItem::portCenter(size_t slot) const {
    return QPointF(slot * TILE_SIZE + TILE_SIZE / 2.0, TILE_SIZE / 2.0);
}
void DeviceItem::addLink(LinkItem* link) {
    links.push_back(link);
}
void DeviceItem::removeLink(LinkItem* link) {
    links.erase(std::remove(links.begin(), links.end(), link), links.end());
}
QVariant DeviceItem::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == QGraphicsItem::ItemPositionHasChanged ||
        change == QGraphicsItem::ItemRotationHasChanged) {
        for (LinkItem* link : links)
            link->update_geometry();
    }

    return QGraphicsItem::itemChange(change, value);
}
QRectF DeviceItem::boundingRect() const {
    return QRectF(0, 0, nslot * TILE_SIZE, TILE_SIZE);
}
void DeviceItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    painter->setPen(Qt::black);
    for (size_t i = 0; i < nslot; i++) {
        if (portHighlightEnabled && i == hoveredPort)
            painter->fillRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE, Qt::yellow);
        painter->drawRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE);
    }
}
void DeviceItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    QPointF pos = event->pos();

    int port = static_cast<int>(pos.x() / TILE_SIZE);

    if (port >= 0 && port < nslot)
        hoveredPort = port;
    else
        hoveredPort = -1;

    update();
    QGraphicsItem::hoverMoveEvent(event);
}
void DeviceItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    hoveredPort = -1;
    update();
    QGraphicsItem::hoverLeaveEvent(event);
}
