#include "computer_item.hpp"
#include "network_view.hpp"
#include "terminal_command/build_tc.hpp"
#include "terminal_widget.hpp"

ComputerItem::ComputerItem(Computer* computer) : computer(computer), DeviceItem(computer, computer->port_count()), terminal(new TerminalWidget<ComputerItem>(this, nullptr)) {
    terminal->hide();
    terminal->shell.command_trie = &computer_command_trie;
    vlanVizColor                 = Qt::transparent;
}
ComputerItem::ComputerItem(int nslot) : computer(nullptr), DeviceItem(nullptr, nslot) {}
QRectF ComputerItem::boundingRect() const {
    return QRectF(-16, -40, 48, 40 + TILE_SIZE);
}
QPointF ComputerItem::portCenter(std::size_t port) const {
    return QPointF(port * TILE_SIZE + TILE_SIZE / 2.0, TILE_SIZE / 2.0);
}
void ComputerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    QColor tint = vlanVizEnabled ? vlanVizColor : Qt::transparent;
    assets.drawComputer(painter, QPointF(-16, -40), tint);
    assets.drawPortEmpty(painter, QPointF(0, 0), tint);
    if (device && device->has_interface(0)) {
        QRect rect(0, 0, 16, 16);
        if (portHighlightEnabled && hoveredPort == 0)
            painter->fillRect(rect, Qt::red);
        else
            painter->fillRect(rect, Qt::black);
        if (device->has_interface_connected(0))
            assets.drawPortRJ45Filled(painter, QPointF(0, 0), tint);
        else
            assets.drawPortRJ45Empty(painter, QPointF(0, 0), tint);
    }
}
void ComputerItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    if (!device)
        return;
    QMenu menu;
    QAction* displayTerminalAction = menu.addAction("Terminal");
    QAction* selectedAction        = menu.exec(event->screenPos());
    if (selectedAction == displayTerminalAction)
        terminal->setVisible(!terminal->isVisible());
    return;
}
void ComputerItem::updateVlanVizColor() {
    if (!device)
        return;
    std::uint16_t vlan = computer->get_vlan();
    if (vlan == 0)
        vlanVizColor = Qt::transparent;
    else
        vlanVizColor = vlanColor(vlan);
    update();
}
void ComputerItem::setVlanVizEnabled(bool enabled) {
    vlanVizEnabled = enabled;
    updateVlanVizColor();
}
bool ComputerItem::removeLink(std::size_t port) {
    if (DeviceItem::removeLink(port)) {
        vlanVizColor = Qt::transparent;
        return true;
    }
    return false;
}
