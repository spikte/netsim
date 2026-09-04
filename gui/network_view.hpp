#ifndef GUI_NETWORK_VIEW_HPP
#define GUI_NETWORK_VIEW_HPP

#include "../include/simulation.hpp"
#include "switch_item.hpp"
#include "computer_item.hpp"
#include "link_item.hpp"
#include "settings.hpp"

#include "state.hpp"
#include <QDebug>
#include <QElapsedTimer>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>

/* == NetworkView == */
struct NetworkView : public QGraphicsView {
    Q_OBJECT

  public:
    enum class Tool {
        Select   = 0,
        Switch   = 1,
        Link     = 2,
        Computer = 3
    };
    InteractionState state;
    Tool tool;
    Simulation simulation;
    bool gridEnabled;
    bool snapToGridEnabled;
    QGraphicsItem* preview;
    std::vector<LinkItem*> linkItems;

    // Time related
    QTimer timer;
    QElapsedTimer clock;
    qint64 lastFrame;

    NetworkView(QWidget* parent = nullptr);
    void onFrame();
    QPointF snapToGrid(QPointF point);
    void setGridEnabled(bool enabled);
    void setSnapToGridEnabled(bool enabled);
    QGraphicsItem* createItem(Tool tool);
    void setTool(Tool tool);
    bool canPlaceItem(QGraphicsItem* item);
    QPointF getMousePosScene(QMouseEvent* event = nullptr);
    void deletePreview();
    void setItemToMousePos(QGraphicsItem* item);
    QGraphicsItem* topDevice(const QPointF& pos);
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void updatePacketDots(double dt);

    // Event
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void scrollContentsBy(int dx, int dy) override;
  signals:
    void globalTickChanged();
};

#endif
