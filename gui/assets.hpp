#ifndef GUI_ASSETS_HPP
#define GUI_ASSETS_HPP

#include <QPainter>
#include <QPixmap>
#include <QPoint>

struct Assets {
    QPixmap* spriteSheet;

    Assets();
    ~Assets();
    void build();
    QPixmap tinted(const QRect& sourceRect, const QColor& color) const;
    void drawTile(QPainter* painter, QPoint source, QPointF position, QColor tint, QPoint dimension = QPoint(16, 16)) const;
    void drawComputer(QPainter* painter, QPointF position, QColor tint) const;
    void drawPortEmpty(QPainter* painter, QPointF position, QColor tint) const;
    void drawPortRJ45Empty(QPainter* painter, QPointF position, QColor tint) const;
    void drawPortRJ45Filled(QPainter* painter, QPointF position, QColor tint) const;
    void drawPortCaseLeft(QPainter* painter, QPointF position, QColor tint) const;
    void drawPortCaseFull(QPainter* painter, QPointF position, QColor tint) const;
    void drawPortCaseRightCorner(QPainter* painter, QPointF position, QColor tint) const;
    void drawPortCaseRight(QPainter* painter, QPointF position, QColor tint) const;
    void drawPortCaseRightLeft(QPainter* painter, QPointF position, QColor tint) const;
    void drawPortCaseTop(QPainter* painter, QPointF position, QColor tint) const;
    void drawPortCase(QPainter* painter, QPointF position, QColor tint, int nport, int depth) const;
    void drawAntennaTop(QPainter* painter, QPointF position, QColor tint) const;
    void drawAntennaMid(QPainter* painter, QPointF position, QColor tint) const;
    void drawAntennaBot(QPainter* painter, QPointF position, QColor tint) const;
    void drawAntenna(QPainter* painter, QPointF position, QColor tint, int height) const;
    void drawIconSwitch(QPainter* painter, QPointF position, QColor tint) const;
    void drawIconRouter(QPainter* painter, QPointF position, QColor tint) const;
    void drawIconComputer(QPainter* painter, QPointF position, QColor tint) const;
    void drawIconLink(QPainter* painter, QPointF position, QColor tint) const;
    void drawIconSelect(QPainter* painter, QPointF position, QColor tint) const;
    void drawIconDelete(QPainter* painter, QPointF position, QColor tint) const;
    QPixmap iconSwitch(QColor tint) const;
    QPixmap iconRouter(QColor tint) const;
    QPixmap iconComputer(QColor tint) const;
    QPixmap iconLink(QColor tint) const;
    QPixmap iconSelect(QColor tint) const;
    QPixmap iconDelete(QColor tint) const;
};
extern Assets assets;

#endif
