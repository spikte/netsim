#include "assets.hpp"

Assets assets;
Assets::Assets() : spriteSheet(nullptr) {}
Assets::~Assets() {
    delete spriteSheet;
}
void Assets::build() {
    delete spriteSheet;
    spriteSheet = new QPixmap(":/sprites.png");
}
QPixmap Assets::tinted(const QRect& sourceRect, const QColor& color) const {
    QPixmap sprite = spriteSheet->copy(sourceRect);

    QPainter p(&sprite);

    // Tint while preserving the sprite's transparency.
    p.setCompositionMode(QPainter::CompositionMode_Multiply);
    p.fillRect(sprite.rect(), color);

    // Restore the original alpha channel.
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.drawPixmap(0, 0, spriteSheet->copy(sourceRect));

    return sprite;
}
void Assets::drawTile(QPainter* painter, QPoint source, QPointF position, QColor tint, QPoint dimension) const {
    QRect sourceRect(source.x(), source.y(), dimension.x(), dimension.y());
    QRect positionRect(position.x(), position.y(), dimension.x(), dimension.y());
    QPixmap sprite = tinted(sourceRect, tint);
    painter->drawPixmap(positionRect, sprite);
}
void Assets::drawComputer(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(0, 32), position, tint, QPoint(48, 40));
}
void Assets::drawPortEmpty(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(0, 0), position, tint);
}
void Assets::drawPortRJ45Empty(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(0, 16), position, tint);
}
void Assets::drawPortRJ45Filled(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(16, 16), position, tint);
}
void Assets::drawPortCaseLeft(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(16, 0), position, tint);
}
void Assets::drawPortCaseFull(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(32, 0), position, tint);
}
void Assets::drawPortCaseRightCorner(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(48, 0), position, tint);
}
void Assets::drawPortCaseRight(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(32, 16), position, tint);
}
void Assets::drawPortCaseRightLeft(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(48, 32), position, tint);
}
void Assets::drawPortCaseTop(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(48, 16), position, tint);
}
void Assets::drawPortCase(QPainter* painter, QPointF position, QColor tint, int nport, int depth) const {
    nport++;
    for (std::size_t j = 0; j < nport; j++) {
        if (j == 0)
            drawPortCaseLeft(painter, position, tint);
        else if (j == nport - 1)
            drawPortCaseRightCorner(painter, QPointF(position.x() + j * 16, position.y()), tint);
        else
            drawPortCaseTop(painter, QPointF(position.x() + j * 16, position.y()), tint);
    }
    for (std::size_t i = 1; i < depth; i++) {
        for (std::size_t j = 0; j < nport; j++) {
            if (j == 0)
                drawPortCaseLeft(painter, QPointF(position.x() - i * 16, position.y() + i * 16), tint);
            else if (j == nport - 1)
                drawPortCaseRightLeft(painter, QPointF(position.x() - i * 16 + j * 16, position.y() + i * 16), tint);
            else
                drawPortCaseFull(painter, QPointF(position.x() - i * 16 + j * 16, position.y() + i * 16), tint);
        }
    }
    for (std::size_t i = 1; i < depth + 1; i++)
        drawPortCaseRight(painter, QPointF(position.x() + nport * 16 - i * 16, position.y() + i * 16), tint);
}
void Assets::drawAntennaTop(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(48, 48), position, tint);
}
void Assets::drawAntennaMid(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(48, 64), position, tint);
}
void Assets::drawAntennaBot(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(48, 80), position, tint);
}
void Assets::drawAntenna(QPainter* painter, QPointF position, QColor tint, int height) const {
    drawAntennaTop(painter, position, tint);
    for (size_t i = 1; i < height - 1; i++)
        drawAntennaMid(painter, QPointF(position.x(), position.y() + i * 16), tint);
    drawAntennaBot(painter, QPointF(position.x(), position.y() + (height - 1) * 16), tint);
}
void Assets::drawIconSwitch(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(0, 80), position, tint);
}
void Assets::drawIconRouter(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(16, 80), position, tint);
}
void Assets::drawIconComputer(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(32, 80), position, tint);
}
void Assets::drawIconLink(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(0, 96), position, tint);
}
void Assets::drawIconSelect(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(16, 96), position, tint);
}
void Assets::drawIconDelete(QPainter* painter, QPointF position, QColor tint) const {
    drawTile(painter, QPoint(32, 96), position, tint);
}
QPixmap Assets::iconSwitch(QColor tint) const {
    return tinted(QRect(0, 80, 16, 16), tint).scaled(48, 48, Qt::IgnoreAspectRatio, Qt::FastTransformation);
}
QPixmap Assets::iconRouter(QColor tint) const {
    return tinted(QRect(16, 80, 16, 16), tint).scaled(48, 48, Qt::IgnoreAspectRatio, Qt::FastTransformation);
}
QPixmap Assets::iconComputer(QColor tint) const {
    return tinted(QRect(32, 80, 16, 16), tint).scaled(48, 48, Qt::IgnoreAspectRatio, Qt::FastTransformation);
}
QPixmap Assets::iconLink(QColor tint) const {
    return tinted(QRect(0, 96, 16, 16), tint).scaled(48, 48, Qt::IgnoreAspectRatio, Qt::FastTransformation);
}
QPixmap Assets::iconSelect(QColor tint) const {
    return tinted(QRect(16, 96, 16, 16), tint).scaled(48, 48, Qt::IgnoreAspectRatio, Qt::FastTransformation);
}
QPixmap Assets::iconDelete(QColor tint) const {
    return tinted(QRect(32, 96, 16, 16), tint).scaled(48, 48, Qt::IgnoreAspectRatio, Qt::FastTransformation);
}
