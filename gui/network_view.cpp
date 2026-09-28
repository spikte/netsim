#include "network_view.hpp"

/* == NetworkView == */
NetworkView::NetworkView(QWidget* parent) : QGraphicsView(parent),
                                            tool(Tool::Select),
                                            gridEnabled(true),
                                            snapToGridEnabled(false),
                                            vlanVizEnabled(false),
                                            state(IdleState()),
                                            preview(nullptr) {
    setScene(new QGraphicsScene(this));
    setMouseTracking(true);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);
    clock.start();
    lastFrame = clock.nsecsElapsed();
    timer.setTimerType(Qt::PreciseTimer);
    connect(&timer, &QTimer::timeout, this, &NetworkView::onFrame);
    timer.start(0);

    auto* router    = addDevice(Tool::Router, {0, -150});
    auto* router2   = addDevice(Tool::Router, {300, -150});
    auto* sw        = addDevice(Tool::Switch, {0, 0});
    auto* sw2       = addDevice(Tool::Switch, {300, 0});
    auto* computer  = addDevice(Tool::Computer, {0, 150});
    auto* computer2 = addDevice(Tool::Computer, {300, 150});

    addLink(computer, 0, sw, 0);
    addLink(sw, 7, router, 0);
    addLink(router, 7, router2, 0);
    addLink(router2, 7, sw2, 7);
    addLink(sw2, 0, computer2, 0);

    auto* routerItem = dynamic_cast<RouterItem*>(router);
    routerItem->device->interface_set_ipv4(0, IPv4CIDR(*str2ipv4("10.0.0.1"), (IPv4Address) *str2ipv4("255.255.255.0")));
    routerItem->device->interface_set_ipv4(7, IPv4CIDR(*str2ipv4("10.0.2.1"), (IPv4Address) *str2ipv4("255.255.255.0")));
    auto* routerItem2 = dynamic_cast<RouterItem*>(router2);
    routerItem2->device->interface_set_ipv4(0, IPv4CIDR(*str2ipv4("10.0.2.2"), (IPv4Address) *str2ipv4("255.255.255.0")));
    routerItem2->device->interface_set_ipv4(7, IPv4CIDR(*str2ipv4("10.0.1.1"), (IPv4Address) *str2ipv4("255.255.255.0")));

    auto* computerItem = dynamic_cast<ComputerItem*>(computer);
    computerItem->device->interface_set_ipv4(0, IPv4CIDR(*str2ipv4("10.0.0.2"), (IPv4Address) *str2ipv4("255.255.255.0")));
    auto* computerItem2 = dynamic_cast<ComputerItem*>(computer2);
    computerItem2->device->interface_set_ipv4(0, IPv4CIDR(*str2ipv4("10.0.1.2"), (IPv4Address) *str2ipv4("255.255.255.0")));

    // Route between router and computer2
    routerItem->device->routing_table->add_entry(
        IPv4CIDR(*str2ipv4("10.0.1.0"), prefix2subnet(24)),
        *str2ipv4("10.0.2.2"),
        "",
        0,
        RoutingTableProtocol::Static,
        1);
    routerItem->device->forwarding_table->build(*routerItem->device->routing_table);
    // Route between router2 and computer
    routerItem2->device->routing_table->add_entry(
        IPv4CIDR(*str2ipv4("10.0.0.0"), prefix2subnet(24)),
        *str2ipv4("10.0.2.1"),
        "",
        0,
        RoutingTableProtocol::Static,
        1);
    routerItem2->device->forwarding_table->build(*routerItem2->device->routing_table);
    // Route between computer and computer2
    computerItem->device->routing_table->add_entry(
        IPv4CIDR(*str2ipv4("10.0.1.0"), prefix2subnet(24)),
        *str2ipv4("10.0.0.1"),
        "",
        0,
        RoutingTableProtocol::Static,
        1);
    computerItem->device->forwarding_table->build(*computerItem->device->routing_table);
    // Route between computer2 and computer
    computerItem2->device->routing_table->add_entry(
        IPv4CIDR(*str2ipv4("10.0.0.0"), prefix2subnet(24)),
        *str2ipv4("10.0.1.1"),
        "",
        0,
        RoutingTableProtocol::Static,
        1);
    computerItem2->device->forwarding_table->build(*computerItem2->device->routing_table);
}

// Setters
void NetworkView::setGridEnabled(bool enabled) {
    gridEnabled = enabled;
    viewport()->update();
}
void NetworkView::setSnapToGridEnabled(bool enabled) {
    snapToGridEnabled = enabled;
}
void NetworkView::setVlanVizEnabled(bool enabled) {
    vlanVizEnabled = enabled;
    for (QGraphicsItem* item : scene()->items()) {
        if (DeviceItem* deviceItem = dynamic_cast<DeviceItem*>(item)) {
            if (!deviceItem->device)
                continue;
            deviceItem->setVlanVizEnabled(enabled);
        }
    }
}
void NetworkView::updateInteractionState() {
    switch (tool) {
        case Tool::Switch:
        case Tool::Computer:
            state = PlacingState(*this);
        case Tool::Router:
            state = PlacingState(*this);
            break;
        case Tool::Link:
            state = LinkingState();
            break;
        case Tool::Select:
            state = SelectState();
            break;
        case Tool::Delete:
            state = DeleteState();
            break;
        default:
            break;
    }
}
void NetworkView::updatePortHighlight() {
    bool portHighlightEnabled = tool == Tool::Link;
    for (QGraphicsItem* item : scene()->items()) {
        if (auto* device = dynamic_cast<DeviceItem*>(item))
            if (device->device) // We don't enabled when the internal Device ref is null (for the preview)
                device->setPortHighlightEnabled(portHighlightEnabled);
    }
}
void NetworkView::deletePreview() {
    if (preview) {
        if (preview->scene())
            scene()->removeItem(preview);
        delete preview;
        preview = nullptr;
    }
}
void NetworkView::setTool(Tool tool) {
    if (tool == this->tool)
        return;
    this->tool = tool;
    deletePreview();
    updateInteractionState();
    updatePortHighlight();
}
void NetworkView::setItemToMousePos(QGraphicsItem* item) {
    QPointF mousePos = getMousePosScene();
    if (snapToGridEnabled)
        item->setPos(snapToGrid(mousePos));
    else
        item->setPos(mousePos);
}
void NetworkView::updatePacketDots(double dt) {
    for (LinkItem* link : linkItems)
        link->updatePacketDots(dt);
}

// Create Item
void NetworkView::addItem(QGraphicsItem* item) {
    scene()->addItem(item);
    updateSceneRect();
}

// Create device
void NetworkView::setupDevice(DeviceItem* item) {
    item->setVlanVizEnabled(vlanVizEnabled);
    item->setPortHighlightEnabled(tool == Tool::Link);
    item->setFlag(QGraphicsItem::ItemIsMovable);
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    item->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}
DeviceItem* NetworkView::createDevice(Tool tool) {
    DeviceItem* item;

    item = nullptr;
    switch (tool) {
        case Tool::Router: {
            Router* router = &simulation.add_router();
            for (std::size_t port = 0; port < router->port_count(); port++)
                router->interface_add(port, std::format("eth{}", port), Device::last_mac_address++);
            item = new RouterItem(router);
            break;
        }
        case Tool::Switch: {
            Switch* sw = &simulation.add_switch();
            for (std::size_t port = 0; port < sw->port_count(); port++)
                sw->interface_add(port, std::format("eth{}", port), Device::last_mac_address++);
            item = new SwitchItem(sw);
            break;
        }
        case Tool::Computer: {
            Computer* computer = &simulation.add_computer();
            computer->interface_add(0, "eth0", Device::last_mac_address++);
            item = new ComputerItem(computer);
            break;
        }
        default:
            break;
    }
    if (item)
        setupDevice(item);
    return item;
}
DeviceItem* NetworkView::addDevice(Tool tool, QPointF position) {
    auto* item = createDevice(tool);
    if (!item)
        return nullptr;
    item->setPos(position);
    addItem(item);
    return item;
}
void NetworkView::deleteDevice(DeviceItem* device) {
    if (!device)
        return;
    for (std::size_t i = 0; i < device->links.size(); i++) {
        LinkItem* linkItem = device->links[i];
        deleteLink(linkItem);
        device->removeLink(i);
    }
    if (device->device) {
        if (auto* computer = dynamic_cast<ComputerItem*>(device))
            simulation.delete_computer(computer->computer);
        else if (auto* sw = dynamic_cast<SwitchItem*>(device))
            simulation.delete_switch(sw->sw);
        else if (auto* router = dynamic_cast<RouterItem*>(device))
            simulation.delete_router(router->router);
    }
    if (device->scene())
        scene()->removeItem(device);
    delete device;
}

// Create/delete link
LinkItem* NetworkView::addLink(DeviceItem* item1, std::size_t port1, DeviceItem* item2, std::size_t port2) {
    if (!item1 || !item2 || !item1->device || !item2->device)
        return nullptr;
    if (!item1->device->has_interface(port1))
        return nullptr;
    if (!item2->device->has_interface(port2))
        return nullptr;

    Link* link = simulation.add_link(*item1->device, port1, *item2->device, port2);
    if (!link)
        return nullptr;
    LinkItem* linkItem = new LinkItem(item1, port1, item2, port2, link);
    linkItem->setZValue(100000);
    linkItem->src->setVlanVizEnabled(vlanVizEnabled);
    linkItem->dst->setVlanVizEnabled(vlanVizEnabled);
    linkItems.push_back(linkItem);
    addItem(linkItem);

    return linkItem;
}
void NetworkView::deleteLink(LinkItem* link) {
    if (!link)
        return;
    std::erase(linkItems, link);
    if (link->link)
        simulation.delete_link(link->link);
    delete link;
}

// Getters
bool NetworkView::canPlaceItem(QGraphicsItem* item) {
    for (QGraphicsItem* other : item->collidingItems()) {
        if (!dynamic_cast<LinkItem*>(other))
            return false;
    }
    return true;
}
QPointF NetworkView::getMousePosScene(QMouseEvent* event) {
    QPointF mousePos;

    if (event) {
        mousePos = mapToScene(event->position().toPoint());
    } else {
        QPoint globalMousePos = viewport()->mapFromGlobal(QCursor::pos());
        mousePos              = mapToScene(globalMousePos);
    }

    return mousePos;
}
QGraphicsItem* NetworkView::topDevice(const QPointF& pos) {
    const auto items = scene()->items(pos,
                                      Qt::IntersectsItemShape,
                                      Qt::DescendingOrder,
                                      transform());
    for (QGraphicsItem* item : items) {
        if (auto* device = dynamic_cast<DeviceItem*>(item))
            return device;
    }
    return nullptr;
}
QGraphicsItem* NetworkView::topItem(const QPointF& pos) {
    const auto items = scene()->items(pos,
                                      Qt::IntersectsItemShape,
                                      Qt::DescendingOrder,
                                      transform());
    if (!items.empty())
        return items[0];
    return nullptr;
}
QPointF NetworkView::snapToGrid(QPointF point) {
    return QPointF(
        std::round(point.x() / TILE_SIZE) * TILE_SIZE,
        std::round(point.y() / TILE_SIZE) * TILE_SIZE);
}

// Draw
void NetworkView::drawBackground(QPainter* painter, const QRectF& rect) {
    painter->save();

    painter->fillRect(rect, QColor("#F7F7F5"));

    if (gridEnabled) {
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor("#DDE2E8"));

        const qreal left =
            std::floor(rect.left() / TILE_SIZE) * TILE_SIZE;

        const qreal top =
            std::floor(rect.top() / TILE_SIZE) * TILE_SIZE;

        constexpr qreal dotSize = 2.0;

        for (qreal x = left; x <= rect.right(); x += TILE_SIZE) {
            for (qreal y = top; y <= rect.bottom(); y += TILE_SIZE) {
                painter->drawRect(
                    QRectF(
                        x - dotSize / 2.0,
                        y - dotSize / 2.0,
                        dotSize,
                        dotSize));
            }
        }
    }

    painter->restore();
}
void NetworkView::updateSceneRect() {
    constexpr qreal margin = 1000.0;

    QRectF bounds = scene()->itemsBoundingRect();

    scene()->setSceneRect(bounds.adjusted(-margin, -margin, margin, margin));
}

// Event
void NetworkView::onFrame() {
    qint64 now;
    double dt;

    now                            = clock.nsecsElapsed();
    dt                             = double(now - lastFrame) / 1'000'000'000.0;
    lastFrame                      = now;
    std::uint64_t prev_global_tick = global_tick;
    simulation.update(dt);
    if (prev_global_tick != global_tick) {
        hasTickAdvanced = true;
        emit onTick();
    } else
        hasTickAdvanced = false;
    updatePacketDots(dt);
}
void NetworkView::wheelEvent(QWheelEvent* event) {
    constexpr double zoomStep = 1.15;

    if (event->angleDelta().y() > 0)
        scale(zoomStep, zoomStep);
    else
        scale(1.0 / zoomStep, 1.0 / zoomStep);

    event->accept();
}
void NetworkView::mousePressEvent(QMouseEvent* event) {
    const bool handled = std::visit(
        [this, event](auto& currentState) {
            return currentState.mousePress(*this, *event);
        },
        state);
    if (!handled)
        QGraphicsView::mousePressEvent(event);
}
void NetworkView::mouseMoveEvent(QMouseEvent* event) {
    const bool handled = std::visit(
        [this, event](auto& currentState) {
            return currentState.mouseMove(*this, *event);
        },
        state);
    if (!handled)
        QGraphicsView::mouseMoveEvent(event);
}
void NetworkView::scrollContentsBy(int dx, int dy) {
    const bool handled = std::visit(
        [this, dx, dy](auto& currentState) {
            return currentState.scrollContent(*this, dx, dy);
        },
        state);
    if (!handled)
        QGraphicsView::scrollContentsBy(dx, dy);
}
