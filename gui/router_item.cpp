#include "router_item.hpp"
#include "terminal_command/build_tc.hpp"
#include "terminal_widget.hpp"

RouterItem::RouterItem(Router* router) : router(router), DeviceItem(router, router->interfaces.size()), terminal(new TerminalWidget<RouterItem>(this, nullptr)) {
    terminal->hide();
    terminal->shell.command_trie = &router_command_trie;
}
RouterItem::RouterItem(int nslot) : router(nullptr), DeviceItem(nullptr, nslot) {
}
void RouterItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    int depth           = 2;
    QPointF topLeftCase = QPointF((depth - 1) * 16, -depth * 16);
    assets.drawPortCase(painter, topLeftCase, Qt::transparent, nport, depth);
    int nAntenna      = 3;
    int antennaHeight = 3;
    for (size_t i = 0; i < nAntenna; i++)
        assets.drawAntenna(painter, QPointF(topLeftCase.x() + (i + 1) * 16, topLeftCase.y() - antennaHeight * 16), Qt::transparent, antennaHeight);
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
QPainterPath RouterItem::shape() const {
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
QRectF RouterItem::boundingRect() const {
    return shape().boundingRect();
}
void RouterItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    if (!device)
        return;
    QMenu menu;
    QAction* displayTerminalAction = menu.addAction("Terminal");
    QAction* selectedAction        = menu.exec(event->screenPos());
    if (selectedAction == displayTerminalAction)
        terminal->setVisible(!terminal->isVisible());
    return;
}
