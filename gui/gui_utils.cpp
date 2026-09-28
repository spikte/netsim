#include "gui_utils.hpp"

QColor vlanColor(int vlanId) {
    int hue = static_cast<int>(std::fmod(vlanId * 137.508, 360.0));
    return QColor::fromHsl(hue, 180, 150);
}
