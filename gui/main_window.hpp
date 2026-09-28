#ifndef GUI_MAIN_WINDOW_HPP
#define GUI_MAIN_WINDOW_HPP

#include "../include/sim/simulation.hpp"
#include "network_view.hpp"
#include "ui_gui.h"

#include <QButtonGroup>
#include <QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

  protected:
    void resizeEvent(QResizeEvent* event) override;

  private:
    void setupTools();
    void positionTools();

    Ui::MainWindow ui;
    QButtonGroup toolGroup;
};

#endif
