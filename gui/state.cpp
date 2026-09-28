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
PlacingState::PlacingState(NetworkView& network) {
    switch (network.tool) {
        case NetworkView::Tool::Switch: {
            network.preview = new SwitchItem(8);
            network.preview->setOpacity(0.5);
            network.preview->setAcceptedMouseButtons(Qt::NoButton);
            network.preview->setZValue(200000);
            network.setItemToMousePos(network.preview);
            network.scene()->addItem(network.preview);
            break;
        }
        case NetworkView::Tool::Computer: {
            network.preview = new ComputerItem(1);
            network.preview->setOpacity(0.5);
            network.preview->setAcceptedMouseButtons(Qt::NoButton);
            network.preview->setZValue(200000);
            network.setItemToMousePos(network.preview);
            network.scene()->addItem(network.preview);
            break;
        }
        case NetworkView::Tool::Router: {
            network.preview = new RouterItem(8);
            network.preview->setOpacity(0.5);
            network.preview->setAcceptedMouseButtons(Qt::NoButton);
            network.preview->setZValue(200000);
            network.setItemToMousePos(network.preview);
            network.scene()->addItem(network.preview);
            break;
        }
        default:
            break;
    }
};
bool PlacingState::mousePress(NetworkView& network, QMouseEvent& event) {
    if (event.button() == Qt::LeftButton && network.canPlaceItem(network.preview))
        QGraphicsItem* item = network.addDevice(network.tool, network.preview->pos());
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

DeleteState::DeleteState() {};
bool DeleteState::mousePress(NetworkView& network, QMouseEvent& event) {
    QGraphicsItem* topItem = network.topItem(network.getMousePosScene());
    if (topItem) {
        if (auto* link = dynamic_cast<LinkItem*>(topItem))
            network.deleteLink(link);
        else if (auto* device = dynamic_cast<DeviceItem*>(topItem))
            network.deleteDevice(device);
        return true;
    }
    return true;
}
bool DeleteState::mouseMove(NetworkView&, QMouseEvent&) {
    return false;
}
bool DeleteState::scrollContent(NetworkView& network, int, int) {
    return false;
}

/* Linking State */
LinkingState::LinkingState() : src(nullptr), srcSlot(0), dst(nullptr), dstSlot(0) {};
int64_t LinkingState::isSwitchComputerLink() {
    if (!src || !dst)
        return -1;
    bool isSrcComputer = false;
    bool isSrcSwitch   = false;
    bool isDstComputer = false;
    bool isDstSwitch   = false;
    if (auto* computer = dynamic_cast<ComputerItem*>(src))
        isSrcComputer = true;
    else if (auto* sw = dynamic_cast<SwitchItem*>(src))
        isSrcSwitch = true;
    if (auto* computer = dynamic_cast<ComputerItem*>(dst))
        isDstComputer = true;
    else if (auto* sw = dynamic_cast<SwitchItem*>(dst))
        isDstSwitch = true;
    if (isSrcComputer && isDstSwitch)
        return 0;
    if (isSrcSwitch && isDstComputer)
        return 1;
    return -1;
}
bool LinkingState::mousePress(NetworkView& network, QMouseEvent& event) {
    QGraphicsItem* raw = network.topDevice(network.getMousePosScene());
    if (auto* dev_item = dynamic_cast<DeviceItem*>(raw)) {
        if (!src) {
            if (dev_item->device && dev_item->device->has_interface(dev_item->hoveredPort) && !dev_item->device->has_interface_connected(dev_item->hoveredPort)) {
                src     = dev_item;
                srcSlot = dev_item->hoveredPort;
                if (!network.preview)
                    network.preview = new LinkItem();
                network.preview->setAcceptedMouseButtons(Qt::NoButton);
                network.preview->setPos(dev_item->mapToScene(dev_item->portCenter(dev_item->hoveredPort)));
                network.preview->setZValue(200000);
                ((LinkItem*) network.preview)->setEnd(network.preview->mapFromScene(network.getMousePosScene()));
                network.addItem(network.preview);
            }
        } else {
            if (dev_item->device && dev_item->device->has_interface(dev_item->hoveredPort) && !dev_item->device->has_interface_connected(dev_item->hoveredPort)) {
                dst        = dev_item;
                dstSlot    = dev_item->hoveredPort;
                auto* item = network.addLink(src, srcSlot, dst, dstSlot);
                network.state = LinkingState();
                network.deletePreview();
                delete network.preview;
                network.preview = nullptr;
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
