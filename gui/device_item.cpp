#include "device_item.hpp"
#include "link_item.hpp"
#include "network_view.hpp"

/* == DeviceItem == */
DeviceItem::DeviceItem(Device* device, int nport) : device(device), nport(nport), hoveredPort(-1), portHighlightEnabled(false), QGraphicsItem() {
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setTransformOriginPoint(boundingRect().center());
}
void DeviceItem::setPortHighlightEnabled(bool enabled) {
    portHighlightEnabled = enabled;
}
QPointF DeviceItem::portCenter(size_t port) const {
    return QPointF(port * TILE_SIZE + TILE_SIZE / 2.0, TILE_SIZE / 2.0);
}
void DeviceItem::addLink(LinkItem* link) {
    links.push_back(link);
}
void DeviceItem::removeLink(LinkItem* link) {
    links.erase(std::remove(links.begin(), links.end(), link), links.end());
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
    if (change == QGraphicsItem::ItemPositionHasChanged) {
        for (LinkItem* link : links)
            link->updateFromDevices();
        return value;
    }
    return QGraphicsItem::itemChange(change, value);
}
QRectF DeviceItem::boundingRect() const {
    return QRectF(0, 0, nport * TILE_SIZE, TILE_SIZE);
}
void DeviceItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    painter->setRenderHint(QPainter::Antialiasing);

    QRectF body                 = boundingRect();
    QColor colorHasInterface    = QColor("#4c9aff");
    QColor colorNotHasInterface = QColor("#b9bec6");

    // Device body
    QLinearGradient grad(body.topLeft(), body.bottomLeft());
    grad.setColorAt(0, QColor("#f5f6f8"));
    grad.setColorAt(1, QColor("#e2e4e8"));
    painter->setBrush(grad);
    painter->setPen(QPen(QColor("#9aa0a8"), 1));
    painter->drawRoundedRect(body, 6, 6);

    // Port separators + indicators
    for (size_t i = 0; i < nport; i++) {
        qreal x = i * TILE_SIZE;
        // Separator
        if (i > 0) {
            painter->setPen(QPen(QColor("#c7cad0"), 1));
            painter->drawLine(QPointF(x, 4), QPointF(x, TILE_SIZE - 4));
        }

        QPointF center = portCenter(i);
        qreal r        = 4.0;
        bool hovered   = portHighlightEnabled && i == hoveredPort;
        QColor color   = colorNotHasInterface;
        if (device && device->has_interface(i))
            color = colorHasInterface;
        painter->setPen(Qt::NoPen);
        painter->setBrush(color);
        painter->drawEllipse(center, hovered ? r + 1.5 : r, hovered ? r + 1.5 : r);
    }

    // Selection glow
    if (hoveredPort != -1 && option->state & QStyle::State_Selected) {
        QColor color = colorNotHasInterface;
        if (device && device->has_interface(hoveredPort))
            color = colorHasInterface;
        QPen pen(color);
        painter->setBrush(Qt::NoBrush);
        painter->setPen(pen);
        painter->drawRoundedRect(body.adjusted(-1.5, -1.5, 1.5, 1.5), 7, 7);
    }
}
void DeviceItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    QPointF pos = event->pos();

    int port = static_cast<int>(pos.x() / TILE_SIZE);

    if (port >= 0 && port < nport)
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
void DeviceItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    QPointF pos = event->pos();
    int port    = static_cast<int>(pos.x() / TILE_SIZE);
    if (port < 0 || port >= nport)
        return;
    QMenu menu;

    QAction* addInterfaceAction;
    QAction* editInterfaceAction;
    QAction* deleteInterfaceAction;
    if (device->has_interface(port)) {
        editInterfaceAction   = menu.addAction("Edit interface");
        deleteInterfaceAction = menu.addAction("Delete interface");
    } else
        addInterfaceAction = menu.addAction("Add interface");
    QAction* generateFrameAction = menu.addAction("Generate frame");
    QAction* selectedAction      = menu.exec(event->screenPos());
    if (selectedAction == addInterfaceAction) {
        InterfaceConfigDialog dialog;
        if (dialog.exec() == QDialog::Accepted) {
            std::string name = dialog.name().toStdString();
            if (!device->interface_add(port, name, dialog.address(), dialog.mtu(), dialog.bufferSize()))
                return;
            Interface* interface = device->get_interface(port);
            interface->on        = true;
            update();
        }
    } else if (selectedAction == editInterfaceAction) {
        Interface* interface = device->get_interface(port);
        InterfaceConfigDialog dialog(interface);
        if (dialog.exec() == QDialog::Accepted) {
            std::string name       = dialog.name().toStdString();
            interface->name        = name;
            interface->address     = dialog.address();
            interface->mtu         = dialog.mtu();
            interface->buffer_size = dialog.bufferSize();
            update();
        }
    } else if (selectedAction == deleteInterfaceAction) {
        device->interface_del(port);
        update();
    } else if (device->has_interface(port) && selectedAction == generateFrameAction) {
        FrameCreationDialog dialog;
        if (dialog.exec() == QDialog::Accepted) {
            ethernet_t eth;
            ethernet_init(&eth);
            ethernetheader_set_source(&eth.header, dialog.src());
            ethernetheader_set_destination(&eth.header, dialog.dst());
            ethernetheader_set_has_tag(&eth.header, dialog.hasTag());
            if (dialog.hasTag())
                ethernetheader_set_tag(&eth.header, dialog.tag());
            ethernetheader_set_eth_type(&eth.header, static_cast<enum EtherType>(dialog.ethertype()));
            PacketRaw packet;
            packet.resize(ethernet_size(&eth));
            ethernet_build_bytes(packet.data(), &eth, false);
            device->send_packet(port, std::move(packet));
        }
    }
}
