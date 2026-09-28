#include "assets.hpp"
#include "main_window.hpp"
#include "terminal_widget.hpp"
#include "terminal_command/build_tc.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    assets.build();
    MainWindow window;
    window.show();
    build_tc_computer(computer_command_trie);
    build_tc_switch(switch_command_trie);
    build_tc_router(router_command_trie);
    return app.exec();
}
