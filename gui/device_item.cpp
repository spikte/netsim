#include "device_item.hpp"
#include "link_item.hpp"
#include "network_view.hpp"

/* == DeviceItem == */
DeviceItem::DeviceItem(Device* device, int nport) : device(device), nport(nport), hovered(false), hoveredPort(-1), portHighlightEnabled(false), vlanVizEnabled(false), QGraphicsItem() {
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setTransformOriginPoint(boundingRect().center());
    if (device) {
        links.resize(device->interfaces.size());
        vlanVizPortColors.resize(device->interfaces.size());
        for (std::size_t i = 0; i < vlanVizPortColors.size(); i++)
            vlanVizPortColors[i] = Qt::transparent;
    }
}
void DeviceItem::setPortHighlightEnabled(bool enabled) {
    portHighlightEnabled = enabled;
}
void DeviceItem::setVlanVizEnabled(bool enabled) {
    vlanVizEnabled = enabled;
}
QPointF DeviceItem::portCenter(std::size_t port) const {
    return QPointF(port * TILE_SIZE + TILE_SIZE / 2.0, TILE_SIZE / 2.0);
}
void DeviceItem::addLink(std::size_t port, LinkItem* link) {
    links[port] = link;
}
bool DeviceItem::removeLink(LinkItem* link) {
    auto it = std::find(links.begin(), links.end(), link);
    if (it != links.end())
        return removeLink(it - links.begin());
    return false;
}
bool DeviceItem::removeLink(std::size_t port) {
    if (port < nport) {
        links[port] = nullptr;
        return true;
    }
    return false;
}
QVariant DeviceItem::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == QGraphicsItem::ItemPositionChange) {
        QPointF newPos = value.toPointF();
        if (scene() && !scene()->views().isEmpty()) {
            if (auto* view = qobject_cast<NetworkView*>(scene()->views().first())) {
                if (view->snapToGridEnabled)
                    newPos = view->snapToGrid(newPos);
            }
        }
        return newPos;
    }
    if (device && change == QGraphicsItem::ItemPositionHasChanged) {
        setZValue(-scenePos().y());
        for (LinkItem* link : links) {
            if (link && link->src && link->dst)
                link->updateFromDevices();
        }
        return value;
    }
    return QGraphicsItem::itemChange(change, value);
}
QRectF DeviceItem::boundingRect() const {
    return QRectF(0, 0, nport * TILE_SIZE, TILE_SIZE);
}
void DeviceItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    painter->setRenderHint(QPainter::Antialiasing);
    for (std::size_t i = 0; i < nport; i++) {
        QPointF pos(i * TILE_SIZE, 0);
        QRect rect(i * TILE_SIZE, 0, 16, 16);
        if (hoveredPort == i)
            painter->fillRect(rect, Qt::red);
        if (device && device->has_interface(i)) {
            if (hoveredPort != i)
                painter->fillRect(rect, Qt::black);
            if (device->has_interface_connected(i))
                assets.drawPortRJ45Filled(painter, pos, QColor(255, 255, 255, 0));
            else
                assets.drawPortRJ45Empty(painter, pos, QColor(255, 255, 255, 0));
        } else
            assets.drawPortEmpty(painter, pos, QColor(255, 255, 255, 0));
    }
}
void DeviceItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    hovered = true;
    update();
    QGraphicsItem::hoverEnterEvent(event);
}
void DeviceItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    QPointF pos = event->pos();

    int port = static_cast<int>(pos.x() / TILE_SIZE);

    if (pos.x() >= 0 && pos.y() >= 0 && port >= 0 && port < nport)
        hoveredPort = port;
    else
        hoveredPort = -1;
    update();
    QGraphicsItem::hoverMoveEvent(event);
}
void DeviceItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    hovered     = false;
    hoveredPort = -1;
    update();
    QGraphicsItem::hoverLeaveEvent(event);
}
NetworkView* DeviceItem::getNetworkView() {
    NetworkView* view;

    view = nullptr;
    if (scene()) {
        const QList<QGraphicsView*> views = scene()->views();
        if (!views.isEmpty())
            view = dynamic_cast<NetworkView*>(views.first());
    }

    return view;
}
QPainterPath DeviceItem::portCollisionShape() const {
    QPainterPath path;
    path.addRect(
        0,
        0,
        nport * TILE_SIZE,
        TILE_SIZE);
    return path;
}
