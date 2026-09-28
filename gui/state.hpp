#ifndef GUI_STATE_HPP
#define GUI_STATE_HPP

#include "device_item.hpp"
#include "terminal_widget.hpp"
#include <QMouseEvent>
#include <QPoint>

struct NetworkView;
struct IdleState {
    IdleState();
    bool mousePress(NetworkView&, QMouseEvent&);
    bool mouseMove(NetworkView&, QMouseEvent&);
    bool scrollContent(NetworkView&, int, int);
};
struct PlacingState {
    PlacingState(NetworkView&);
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
struct DeleteState {
    DeleteState();
    bool mousePress(NetworkView&, QMouseEvent&);
    bool mouseMove(NetworkView&, QMouseEvent&);
    bool scrollContent(NetworkView&, int, int);
};
struct LinkingState {
    DeviceItem* src;
    std::size_t srcSlot;
    DeviceItem* dst;
    std::size_t dstSlot;

    LinkingState();
    int64_t isSwitchComputerLink();
    bool mousePress(NetworkView&, QMouseEvent&);
    bool mouseMove(NetworkView&, QMouseEvent&);
    bool scrollContent(NetworkView&, int, int);
};
using InteractionState = std::variant<IdleState,
                                      PlacingState,
                                      SelectState,
                                      DeleteState,
                                      LinkingState>;

#endif
