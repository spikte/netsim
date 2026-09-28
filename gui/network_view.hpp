#ifndef GUI_NETWORK_VIEW_HPP
#define GUI_NETWORK_VIEW_HPP

#include "../include/sim/simulation.hpp"
#include "computer_item.hpp"
#include "link_item.hpp"
#include "settings.hpp"
#include "switch_item.hpp"

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
        Router   = 0,
        Switch   = 1,
        Computer = 2,
        Link     = 3,
        Select   = 4,
        Delete   = 5
    };
    InteractionState state;
    Tool tool;
    Simulation simulation;
    bool gridEnabled;
    bool snapToGridEnabled;
    bool vlanVizEnabled;
    QGraphicsItem* preview;
    std::vector<LinkItem*> linkItems;

    // Time related
    QTimer timer;
    QElapsedTimer clock;
    qint64 lastFrame;

    NetworkView(QWidget* parent = nullptr);

    // Setters
    void setGridEnabled(bool enabled);
    void setSnapToGridEnabled(bool enabled);
    void setVlanVizEnabled(bool enabled);
    void updateInteractionState();
    void updatePortHighlight();
    void deletePreview();
    void setTool(Tool tool);
    void setItemToMousePos(QGraphicsItem* item);
    void updatePacketDots(double dt);

    // Create Item
    void addItem(QGraphicsItem* item);

    // Create/delete device
    void setupDevice(DeviceItem* item);
    DeviceItem* createDevice(Tool tool);
    DeviceItem* addDevice(Tool tool, QPointF position);
    void deleteDevice(DeviceItem* device);

    // Create/delete link
    LinkItem* addLink(DeviceItem* item1, std::size_t port1, DeviceItem* item2, std::size_t port2);
    void deleteLink(LinkItem* link);

    // Getters
    bool canPlaceItem(QGraphicsItem* item);
    QPointF getMousePosScene(QMouseEvent* event = nullptr);
    QGraphicsItem* topDevice(const QPointF& pos);
    QGraphicsItem* topItem(const QPointF& pos);
    QPointF snapToGrid(QPointF point);

    // Draw
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void updateSceneRect();

    // Event
    void onFrame();
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void scrollContentsBy(int dx, int dy) override;
  signals:
    void onTick();
};

#endif
