#ifndef GUI_MAIN_WINDOW_HPP
#define GUI_MAIN_WINDOW_HPP

#include "ui_gui.h"
#include "network_view.hpp"
#include "../include/simulation.hpp"
#include <QMainWindow>
#include <QButtonGroup>
#include <QLCDNumber>


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
