#include "network_view.hpp"

NetworkView::NetworkView(QWidget* parent) : QGraphicsView(parent), tool(Tool::Select), gridEnabled(false), snapTogridEnabled(false), state(IdleState()), preview(nullptr) {
    setScene(new QGraphicsScene(this));
    setMouseTracking(true);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
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
    snapTogridEnabled = enabled;
}
QGraphicsItem* NetworkView::createItem(Tool tool) {
    switch (tool) {
        case Tool::Switch: {
            Switch* sw = &simulation.add_switch();
            return new SwitchItem(sw);
        }
        case Tool::Link:
            if (auto* linkingState = std::get_if<LinkingState>(&state)) {
                Link& link         = simulation.addLink(*linkingState->src->device, linkingState->srcSlot, *linkingState->dst->device, linkingState->dstSlot);
                LinkItem* linkItem = new LinkItem(linkingState->src, linkingState->srcSlot, linkingState->dst, linkingState->dstSlot, &link);
                return linkItem;
            }
            return nullptr;
        default:
            return nullptr;
    }
}
void NetworkView::setItemToMousePos(QGraphicsItem* item) {
    QPointF mousePos = getMousePosScene();
    if (snapTogridEnabled)
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
        case Tool::Link: {
            state = LinkingState();
            break;
        }
        case Tool::Select:
            state = MovingState();
            break;
        default:
            break;
    }
    bool portHighlightEnabled = tool == Tool::Link;
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
    painter->fillRect(rect, Qt::white);

    if (gridEnabled) {
        painter->setPen(QPen(Qt::lightGray, 1));

        qreal left = std::floor(rect.left() / TILE_SIZE) * TILE_SIZE;
        qreal top  = std::floor(rect.top() / TILE_SIZE) * TILE_SIZE;

        for (qreal x = left; x <= rect.right(); x += TILE_SIZE)
            painter->drawLine(QPointF(x, rect.top()),
                              QPointF(x, rect.bottom()));

        for (qreal y = top; y <= rect.bottom(); y += TILE_SIZE)
            painter->drawLine(QPointF(rect.left(), y),
                              QPointF(rect.right(), y));
    }
    painter->restore();
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

bool NetworkView::scrollContentByIdle(int dx, int dy) {
    return false;
}
bool NetworkView::scrollContentByPlacing(int dx, int dy) {
    if (preview)
        setItemToMousePos(preview);
    return false;
}
bool NetworkView::scrollContentByMoving(int dx, int dy) {
    return false;
}
bool NetworkView::scrollContentByLinking(int dx, int dy) {
    if (preview) {
        auto* link = static_cast<LinkItem*>(preview);
        link->setEnd(link->mapFromScene(getMousePosScene()));
        link->update();
    }
    return false;
}
void NetworkView::scrollContentsBy(int dx, int dy) {
    QGraphicsView::scrollContentsBy(dx, dy);
}
