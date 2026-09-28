#include "switch_item.hpp"
#include "network_view.hpp"
#include "terminal_command/build_tc.hpp"
#include "terminal_widget.hpp"

SwitchItem::SwitchItem(Switch* sw) : sw(sw), DeviceItem(sw, (*sw->ports).size()), terminal(new TerminalWidget<SwitchItem>(this, nullptr)) {
    terminal->hide();
    terminal->shell.command_trie = &switch_command_trie;
}
SwitchItem::SwitchItem(int nslot) : sw(nullptr), DeviceItem(nullptr, nslot) {
}
void SwitchItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    if (!device)
        return;
    QMenu menu;
    QAction* displayTerminalAction = menu.addAction("Terminal");
    QAction* selectedAction        = menu.exec(event->screenPos());
    if (selectedAction == displayTerminalAction)
        terminal->setVisible(!terminal->isVisible());
    return;
}
void SwitchItem::setVlanVizEnabled(bool enabled) {
    vlanVizEnabled = enabled;
    if (!device)
        return;
    for (std::size_t port = 0; port < device->interfaces.size(); port++)
        updateVlanVizPort(port);
    update();
}
void SwitchItem::updateVlanVizPort(std::size_t port) {
    if (!sw->is_valid_port(port))
        return;
    if ((*sw->ports)[port].type == SwitchPortType::ACCESS) {
        vlanVizPortColors[port] = vlanColor((*sw->ports)[port].access_vlan);
        if (links[port]) {
            ComputerItem* computerItem;
            if (links[port]->src == this)
                computerItem = dynamic_cast<ComputerItem*>(links[port]->dst);
            else
                computerItem = dynamic_cast<ComputerItem*>(links[port]->src);
            if (computerItem)
                computerItem->updateVlanVizColor();
        }
    } else
        vlanVizPortColors[port] = Qt::transparent;
}
bool SwitchItem::portSetType(std::size_t port, SwitchPortType type) {
    if (!SwitchingProcessor::port_set_type(*sw, port, type))
        return false;
    updateVlanVizPort(port);
    update();
    return true;
}
bool SwitchItem::portSetType(const std::string& name, SwitchPortType type) {
    auto it = sw->name_to_port.find(name);
    if (it == sw->name_to_port.end())
        return false;
    return portSetType(it->second, type);
}
bool SwitchItem::portAddVlan(std::size_t port, std::uint16_t vlan) {
    if (!SwitchingProcessor::port_add_vlan(*sw, port, vlan))
        return false;
    updateVlanVizPort(port);
    update();
    return true;
}
bool SwitchItem::portAddVlan(const std::string& name, std::uint16_t vlan) {
    auto it = sw->name_to_port.find(name);
    if (it == sw->name_to_port.end())
        return false;
    return portAddVlan(it->second, vlan);
}
bool SwitchItem::portRmVlan(std::size_t port, std::uint16_t vlan) {
    if (!SwitchingProcessor::port_rm_vlan(*sw, port, vlan))
        return false;
    updateVlanVizPort(port);
    update();
    return true;
}
bool SwitchItem::portRmVlan(const std::string& name, std::uint16_t vlan) {
    auto it = sw->name_to_port.find(name);
    if (it == sw->name_to_port.end())
        return false;
    return portRmVlan(it->second, vlan);
}
void SwitchItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    int depth = 2;
    assets.drawPortCase(painter, QPointF((depth - 1) * 16, -depth * 16), Qt::transparent, nport, depth);
    for (std::size_t i = 0; i < nport; i++) {
        QColor tint = vlanVizEnabled ? vlanVizPortColors[i] : Qt::transparent;
        QPointF pos(i * TILE_SIZE, 0);
        QRect rect(i * TILE_SIZE, 0, 16, 16);
        if (portHighlightEnabled && hoveredPort == i)
            painter->fillRect(rect, Qt::red);
        else
            painter->fillRect(rect, Qt::black);
        if (device && device->has_interface(i)) {
            if (device->has_interface_connected(i))
                assets.drawPortRJ45Filled(painter, pos, tint);
            else
                assets.drawPortRJ45Empty(painter, pos, tint);
        } else
            assets.drawPortEmpty(painter, pos, tint);
    }
    if (hovered) {
        painter->save();
        QPen pen(QColor("#3D8ED0"));
        pen.setWidth(2);
        pen.setJoinStyle(Qt::MiterJoin);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(shape());
        painter->restore();
    }
}
QPainterPath SwitchItem::shape() const {
    QPainterPath path;

    path.moveTo(TILE_SIZE * 2, -TILE_SIZE * 2);
    path.lineTo(TILE_SIZE * 2 + TILE_SIZE * nport, -TILE_SIZE * 2);
    path.lineTo(TILE_SIZE * 2 + TILE_SIZE * nport, -TILE_SIZE);
    path.lineTo(TILE_SIZE * nport, TILE_SIZE);
    path.lineTo(0, TILE_SIZE);
    path.lineTo(0, 0);
    path.closeSubpath();

    return path;
}
QRectF SwitchItem::boundingRect() const {
    return shape().boundingRect();
}
