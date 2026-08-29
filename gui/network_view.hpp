#ifndef GUI_NETWORK_VIEW_HPP
#define GUI_NETWORK_VIEW_HPP

#include "../include/simulation.hpp"
#include "gui_link.hpp"
#include "gui_switch.hpp"
#include "settings.hpp"

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPainter>
#include "state.hpp"

struct NetworkView : public QGraphicsView {
    Q_OBJECT

  public:
    enum class Tool {
        Select = 0,
        Switch = 1,
        Link   = 2
    };
    InteractionState state;
    Tool tool;
    Simulation simulation;
    bool gridEnabled;
    bool snapTogridEnabled;
    QGraphicsItem* preview;


    NetworkView(QWidget* parent = nullptr);
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

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    bool scrollContentByIdle(int dx, int dy);
    bool scrollContentByPlacing(int dx, int dy);
    bool scrollContentByMoving(int dx, int dy);
    bool scrollContentByLinking(int dx, int dy);
    void scrollContentsBy(int dx, int dy) override;
};

#endif
