#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include "ui_gui.h"
#include "network_view.hpp"
#include <QMainWindow>
#include <QButtonGroup>


class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

  private:
    Ui::MainWindow ui;
    QButtonGroup toolGroup;
};

#endif
