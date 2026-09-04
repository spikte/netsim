#ifndef GUI_STATE_HPP
#define GUI_STATE_HPP

#include "device_item.hpp"
#include <QPoint>
#include <QMouseEvent>

struct NetworkView;
struct IdleState {
    IdleState();
    bool mousePress(NetworkView&, QMouseEvent&);
    bool mouseMove(NetworkView&, QMouseEvent&);
    bool scrollContent(NetworkView&, int, int);
};
struct PlacingState {
    PlacingState();
    bool mousePress(NetworkView&, QMouseEvent&);
    bool mouseMove(NetworkView&, QMouseEvent&);
    bool scrollContent(NetworkView&, int, int);
};
struct SelectState {
    SelectState();
    bool mousePress(NetworkView&, QMouseEvent&);
    bool mouseMove(NetworkView&, QMouseEvent&);
    bool scrollContent(NetworkView&, int, int);
};
struct LinkingState {
    DeviceItem* src;
    size_t srcSlot;
    DeviceItem* dst;
    size_t dstSlot;

    LinkingState();
    bool mousePress(NetworkView&, QMouseEvent&);
    bool mouseMove(NetworkView&, QMouseEvent&);
    bool scrollContent(NetworkView&, int, int);
};
using InteractionState = std::variant<IdleState,
                                      PlacingState,
                                      SelectState,
                                      LinkingState>;

#endif
