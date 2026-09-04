#include "utils.hpp"
#include "main_window.hpp"
#include "terminal_widget.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    build_computer_command_trie();
    return app.exec();
}
