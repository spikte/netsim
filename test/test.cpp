#include "test_interface.hpp"

int main() {
    test_interface();

    return soft_assert::failed ? 1 : 0;
}
