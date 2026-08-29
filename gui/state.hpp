#ifndef GUI_STATE_HPP
#define GUI_STATE_HPP

#include "gui_device.hpp"
#include <QPoint>
#include <QMouseEvent>

struct NetworkView;
struct IdleState {
    IdleState();
    bool mousePress(NetworkView&, QMouseEvent&);
    bool mouseMove(NetworkView&, QMouseEvent&);
};
struct PlacingState {
    PlacingState();
    bool mousePress(NetworkView&, QMouseEvent&);
    bool mouseMove(NetworkView&, QMouseEvent&);
};
struct MovingState {
    QPointF offset;

    MovingState();
    bool mousePress(NetworkView&, QMouseEvent&);
    bool mouseMove(NetworkView&, QMouseEvent&);
};
struct LinkingState {
    DeviceItem* src;
    size_t srcSlot;
    DeviceItem* dst;
    size_t dstSlot;

    LinkingState();
    bool mousePress(NetworkView&, QMouseEvent&);
    bool mouseMove(NetworkView&, QMouseEvent&);
};
using InteractionState = std::variant<IdleState,
                                      PlacingState,
                                      MovingState,
                                      LinkingState>;

#endif
