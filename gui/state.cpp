#include "state.hpp"
#include "network_view.hpp"

/* Idle State */
IdleState::IdleState() {};
bool IdleState::mousePress(NetworkView&, QMouseEvent&) {
    return false;
}
bool IdleState::mouseMove(NetworkView&, QMouseEvent&) {
    return false;
}

/* Placing State */
PlacingState::PlacingState() {};
bool PlacingState::mousePress(NetworkView& network, QMouseEvent& event) {
    if (event.button() == Qt::LeftButton && network.canPlaceItem(network.preview)) {
        qDebug() << "Can place\n";
        QGraphicsItem* item = network.createItem(network.tool);
        qDebug() << "Switch nslot = " << ((SwitchItem*) item)->nslot << "\n";
        item->setPos(network.preview->pos());
        network.scene()->addItem(item);
        return true;
    }
    return false;
}
bool PlacingState::mouseMove(NetworkView& network, QMouseEvent& event) {
    if (network.preview)
        network.setItemToMousePos(network.preview);
    return false;
}

/* Moving State */
MovingState::MovingState() : offset(0, 0) {};
bool MovingState::mousePress(NetworkView&, QMouseEvent&) {
    return false;
}
bool MovingState::mouseMove(NetworkView&, QMouseEvent&) {
    return false;
}

/* Linking State */
LinkingState::LinkingState() : src(nullptr), srcSlot(0), dst(nullptr), dstSlot(0) {};
bool LinkingState::mousePress(NetworkView& network, QMouseEvent& event) {
    QGraphicsItem* raw = network.topDevice(network.getMousePosScene());
    if (auto* dev_item = dynamic_cast<DeviceItem*>(raw)) {
        if (!src) {
            src             = dev_item;
            srcSlot         = dev_item->hoveredPort;
            network.preview = new LinkItem();
            network.preview->setPos(dev_item->mapToScene(dev_item->portCenter(dev_item->hoveredPort)));
            ((LinkItem*) network.preview)->setEnd(network.preview->mapFromScene(network.getMousePosScene()));
            network.scene()->addItem(network.preview);
        } else {
            dst        = dev_item;
            dstSlot    = dev_item->hoveredPort;
            auto* item = network.createItem(network.tool);
            if (item)
                network.scene()->addItem(item);
            network.state = LinkingState();
            network.deletePreview();
            network.preview = new LinkItem();
            network.preview->setAcceptedMouseButtons(Qt::NoButton);
        }
        return true;
    }
    return false;
}
bool LinkingState::mouseMove(NetworkView& network, QMouseEvent&) {
    if (network.preview)
        ((LinkItem*) network.preview)->setEnd(network.preview->mapFromScene(network.getMousePosScene()));
    return false;
}
