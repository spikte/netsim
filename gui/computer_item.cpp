#include "computer_item.hpp"
#include "network_view.hpp"

ComputerItem::ComputerItem(Computer* computer) : DeviceItem(computer, computer->port_count()), terminal(computer, nullptr) {
    if (scene() && !scene()->views().isEmpty()) {
        if (auto* view = qobject_cast<NetworkView*>(scene()->views().first()))
            terminal.setParent(view);
    }
    terminal.hide();
    terminal.shell.command_trie = &computer_command_trie;
}
ComputerItem::ComputerItem(int nslot) : DeviceItem(nullptr, nslot) {
    terminal.hide();
}
QRectF ComputerItem::boundingRect() const {
    constexpr qreal COMPUTER_HEIGHT = 55.0;
    return QRectF(0,
                  -COMPUTER_HEIGHT,
                  nport * TILE_SIZE,
                  COMPUTER_HEIGHT + TILE_SIZE);
}
QPointF ComputerItem::portCenter(size_t port) const {
    return QPointF(port * TILE_SIZE + TILE_SIZE / 2.0, TILE_SIZE / 2.0);
}
void ComputerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    painter->setRenderHint(QPainter::Antialiasing);

    constexpr qreal COMPUTER_HEIGHT = 55.0;
    constexpr qreal SCREEN_MARGIN   = 8.0;

    QColor colorHasInterface    = QColor("#4c9aff");
    QColor colorNotHasInterface = QColor("#b9bec6");

    /*
     * ============================================================
     * Computer
     * ============================================================
     */

    QRectF computerBody(
        5,
        -COMPUTER_HEIGHT,
        nport * TILE_SIZE - 10,
        COMPUTER_HEIGHT - 8);

    // Monitor gradient
    QLinearGradient monitorGrad(
        computerBody.topLeft(),
        computerBody.bottomLeft());

    monitorGrad.setColorAt(0, QColor("#f5f6f8"));
    monitorGrad.setColorAt(1, QColor("#dfe2e7"));

    painter->setBrush(monitorGrad);
    painter->setPen(QPen(QColor("#8f959e"), 1.2));
    painter->drawRoundedRect(computerBody, 7, 7);

    /*
     * Screen
     */
    QRectF screen = computerBody.adjusted(
        SCREEN_MARGIN,
        SCREEN_MARGIN,
        -SCREEN_MARGIN,
        -18);

    painter->setBrush(QColor("#303740"));
    painter->setPen(QPen(QColor("#707780"), 1));
    painter->drawRoundedRect(screen, 3, 3);

    // Small screen highlight
    QRectF screenHighlight = screen.adjusted(4, 4, -4, -4);

    QLinearGradient screenGrad(
        screenHighlight.topLeft(),
        screenHighlight.bottomLeft());

    screenGrad.setColorAt(0, QColor("#4c9aff"));
    screenGrad.setColorAt(1, QColor("#26384f"));

    painter->setBrush(screenGrad);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(screenHighlight, 2, 2);

    /*
     * Monitor stand
     */
    qreal centerX = computerBody.center().x();

    painter->setPen(QPen(QColor("#8f959e"), 2));
    painter->drawLine(
        QPointF(centerX, computerBody.bottom() - 8),
        QPointF(centerX, computerBody.bottom() + 1));

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor("#8f959e"));

    painter->drawRoundedRect(
        QRectF(centerX - 13,
               computerBody.bottom() - 1,
               26,
               4),
        2,
        2);

    /*
     * ============================================================
     * Interface row
     * ============================================================
     */

    QRectF portBody(
        0,
        0,
        nport * TILE_SIZE,
        TILE_SIZE);

    QLinearGradient portGrad(
        portBody.topLeft(),
        portBody.bottomLeft());

    portGrad.setColorAt(0, QColor("#f5f6f8"));
    portGrad.setColorAt(1, QColor("#e2e4e8"));

    painter->setBrush(portGrad);
    painter->setPen(QPen(QColor("#9aa0a8"), 1));

    painter->drawRoundedRect(
        portBody,
        6,
        6);

    /*
     * Port separators + indicators
     */
    for (size_t i = 0; i < nport; i++) {
        qreal x = i * TILE_SIZE;

        if (i > 0) {
            painter->setPen(
                QPen(QColor("#c7cad0"), 1));

            painter->drawLine(
                QPointF(x, 4),
                QPointF(x, TILE_SIZE - 4));
        }

        QPointF center = portCenter(i);

        qreal r = 4.0;

        bool hovered =
            portHighlightEnabled &&
            static_cast<int>(i) == hoveredPort;

        QColor color = colorNotHasInterface;

        if (device && device->has_interface(i))
            color = colorHasInterface;

        painter->setPen(Qt::NoPen);
        painter->setBrush(color);

        painter->drawEllipse(
            center,
            hovered ? r + 1.5 : r,
            hovered ? r + 1.5 : r);
    }

    /*
     * Selection / hovered-port highlight
     */
    if (hoveredPort != -1 &&
        option->state & QStyle::State_Selected) {

        QColor color = colorNotHasInterface;

        if (device &&
            device->has_interface(hoveredPort)) {
            color = colorHasInterface;
        }

        QPen pen(color);
        painter->setBrush(Qt::NoBrush);
        painter->setPen(pen);

        painter->drawRoundedRect(
            portBody.adjusted(
                -1.5,
                -1.5,
                1.5,
                1.5),
            7,
            7);
    }
}
void ComputerItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    QPointF pos = event->pos();

    bool onPortRow =
        pos.y() >= 0 &&
        pos.y() <= TILE_SIZE &&
        pos.x() >= 0 &&
        pos.x() < nport * TILE_SIZE;

    if (!onPortRow) {
        QMenu menu;

        QAction* displayTerminalAction = menu.addAction("Terminal");

        QAction* selectedAction =
            menu.exec(event->screenPos());

        if (selectedAction == displayTerminalAction)
            terminal.setVisible(!terminal.isVisible());

        return;
    }

    DeviceItem::contextMenuEvent(event);
}
