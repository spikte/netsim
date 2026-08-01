#include "test_interface.hpp"
#include "test_switch.hpp"

int main() {
    test_interface();
    test_switch();

    return soft_assert::failed ? 1 : 0;
}
