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
bool IdleState::scrollContent(NetworkView&, int, int) {
    return false;
}

/* Placing State */
PlacingState::PlacingState() {};
bool PlacingState::mousePress(NetworkView& network, QMouseEvent& event) {
    if (event.button() == Qt::LeftButton && network.canPlaceItem(network.preview)) {
        QGraphicsItem* item = network.createItem(network.tool);
        item->setPos(network.preview->pos());
        network.scene()->addItem(item);
    }
    return true;
}
bool PlacingState::mouseMove(NetworkView& network, QMouseEvent& event) {
    if (network.preview)
        network.setItemToMousePos(network.preview);
    return false;
}
bool PlacingState::scrollContent(NetworkView& network, int, int) {
    if (network.preview)
        network.setItemToMousePos(network.preview);
    return false;
}

/* Moving State */
SelectState::SelectState() {};
bool SelectState::mousePress(NetworkView& network, QMouseEvent& event) {
    return false;
}
bool SelectState::mouseMove(NetworkView&, QMouseEvent&) {
    return false;
}
bool SelectState::scrollContent(NetworkView& network, int, int) {
    return false;
}

/* Linking State */
LinkingState::LinkingState() : src(nullptr), srcSlot(0), dst(nullptr), dstSlot(0) {};
bool LinkingState::mousePress(NetworkView& network, QMouseEvent& event) {
    QGraphicsItem* raw = network.topDevice(network.getMousePosScene());
    if (auto* dev_item = dynamic_cast<DeviceItem*>(raw)) {
        if (!src) {
            if (dev_item->device && dev_item->device->has_interface(dev_item->hoveredPort)) {
                src             = dev_item;
                srcSlot         = dev_item->hoveredPort;
                network.preview = new LinkItem();
                network.preview->setPos(dev_item->mapToScene(dev_item->portCenter(dev_item->hoveredPort)));
                ((LinkItem*) network.preview)->setEnd(network.preview->mapFromScene(network.getMousePosScene()));
                network.scene()->addItem(network.preview);
            }
        } else {
            if (dev_item->device && dev_item->device->has_interface(dev_item->hoveredPort)) {
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
        }
    }
    return true;
}
bool LinkingState::mouseMove(NetworkView& network, QMouseEvent&) {
    if (network.preview)
        ((LinkItem*) network.preview)->setEnd(network.preview->mapFromScene(network.getMousePosScene()));
    return false;
}
bool LinkingState::scrollContent(NetworkView& network, int, int) {
    if (network.preview) {
        auto* link = static_cast<LinkItem*>(network.preview);
        link->setEnd(link->mapFromScene(network.getMousePosScene()));
        link->update();
    }
    return false;
}
