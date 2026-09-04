#include "network_view.hpp"

/* == NetworkView == */
NetworkView::NetworkView(QWidget* parent) : QGraphicsView(parent), tool(Tool::Select), gridEnabled(false), snapToGridEnabled(false), state(IdleState()), preview(nullptr) {
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
}
void NetworkView::onFrame() {
    qint64 now;
    double dt;

    now                       = clock.nsecsElapsed();
    dt                        = double(now - lastFrame) / 1'000'000'000.0;
    lastFrame                 = now;
    uint64_t prev_global_tick = global_tick;
    simulation.update(dt);
    if (prev_global_tick != global_tick)
        hasTickAdvanced = true;
    else
        hasTickAdvanced = false;
    emit globalTickChanged();
    updatePacketDots(dt);
}
QPointF NetworkView::snapToGrid(QPointF point) {
    return QPointF(
        std::round(point.x() / TILE_SIZE) * TILE_SIZE,
        std::round(point.y() / TILE_SIZE) * TILE_SIZE);
}
void NetworkView::setGridEnabled(bool enabled) {
    gridEnabled = enabled;
    viewport()->update();
}
void NetworkView::setSnapToGridEnabled(bool enabled) {
    snapToGridEnabled = enabled;
}
QGraphicsItem* NetworkView::createItem(Tool tool) {
    switch (tool) {
        case Tool::Switch: {
            Switch* sw = &simulation.add_switch();
            auto* item = new SwitchItem(sw);
            item->setFlag(QGraphicsItem::ItemIsMovable);
            item->setFlag(QGraphicsItem::ItemIsSelectable);
            item->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
            return item;
        }
        case Tool::Computer: {
            Computer* computer = &simulation.add_computer();
            auto* item         = new ComputerItem(computer);
            item->setFlag(QGraphicsItem::ItemIsMovable);
            item->setFlag(QGraphicsItem::ItemIsSelectable);
            item->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
            return item;
        }
        case Tool::Link:
            if (auto* linkingState = std::get_if<LinkingState>(&state)) {
                Link& link             = simulation.addLink(*linkingState->src->device, linkingState->srcSlot, *linkingState->dst->device, linkingState->dstSlot);
                LinkItem* linkItem     = new LinkItem(linkingState->src, linkingState->srcSlot, linkingState->dst, linkingState->dstSlot, &link);
                Interface* intSrc      = linkingState->src->device->get_interface(linkingState->srcSlot);
                Interface* intDst      = linkingState->dst->device->get_interface(linkingState->dstSlot);
                intSrc->on_packet_sent = [linkItem](InTransitPacket* packet) { linkItem->sendPacket(packet, false); };
                intDst->on_packet_sent = [linkItem](InTransitPacket* packet) { linkItem->sendPacket(packet, true); };
                linkItems.push_back(linkItem);
                return linkItem;
            }
            return nullptr;
        default:
            return nullptr;
    }
}
void NetworkView::setItemToMousePos(QGraphicsItem* item) {
    QPointF mousePos = getMousePosScene();
    if (snapToGridEnabled)
        item->setPos(snapToGrid(mousePos));
    else
        item->setPos(mousePos);
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
void NetworkView::setTool(Tool tool) {
    if (tool == this->tool)
        return;
    this->tool = tool;

    deletePreview();
    switch (tool) {
        case Tool::Switch: {
            state   = PlacingState();
            preview = new SwitchItem(8);
            preview->setOpacity(0.5);
            preview->setAcceptedMouseButtons(Qt::NoButton);
            setItemToMousePos(preview);
            scene()->addItem(preview);
            break;
        }
        case Tool::Computer: {
            state   = PlacingState();
            preview = new ComputerItem(1);
            preview->setOpacity(0.5);
            preview->setAcceptedMouseButtons(Qt::NoButton);
            setItemToMousePos(preview);
            scene()->addItem(preview);
            break;
        }
        case Tool::Link: {
            state = LinkingState();
            break;
        }
        case Tool::Select:
            state = SelectState();
            break;
        default:
            break;
    }
    bool portHighlightEnabled = tool == Tool::Link || tool == Tool::Select;
    for (QGraphicsItem* item : scene()->items()) {
        if (auto* device = dynamic_cast<DeviceItem*>(item))
            if (device->device) // We don't enabled when the internal Switch ref is null (for the preview)
                device->setPortHighlightEnabled(portHighlightEnabled);
    }
}
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
void NetworkView::deletePreview() {
    if (preview) {
        if (preview->scene())
            scene()->removeItem(preview);
        delete preview;
        preview = nullptr;
    }
}
void NetworkView::drawBackground(QPainter* painter, const QRectF& rect) {
    painter->save();
    painter->fillRect(rect, QColor("#fafafa"));

    if (gridEnabled) {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor("#d5d7db"));

        qreal left = std::floor(rect.left() / TILE_SIZE) * TILE_SIZE;
        qreal top  = std::floor(rect.top() / TILE_SIZE) * TILE_SIZE;
        qreal dotR = 1.2;

        for (qreal x = left; x <= rect.right(); x += TILE_SIZE)
            for (qreal y = top; y <= rect.bottom(); y += TILE_SIZE)
                painter->drawEllipse(QPointF(x, y), dotR, dotR);
    }
    painter->restore();
}
void NetworkView::updatePacketDots(double dt) {
    for (LinkItem* link : linkItems)
        link->updatePacketDots(dt);
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
