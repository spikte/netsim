#include "main_window.hpp"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), toolGroup(this) {
    ui.setupUi(this);
    toolGroup.addButton(ui.selectButton, static_cast<int>(NetworkView::Tool::Select));
    toolGroup.addButton(ui.switchButton, static_cast<int>(NetworkView::Tool::Switch));
    toolGroup.addButton(ui.computerButton, static_cast<int>(NetworkView::Tool::Computer));
    toolGroup.addButton(ui.linkButton, static_cast<int>(NetworkView::Tool::Link));
    toolGroup.setExclusive(true);
    connect(
        &toolGroup,
        &QButtonGroup::idClicked,
        this,
        [this](int id) { ui.networkView->setTool(static_cast<NetworkView::Tool>(id)); });

    connect(
        ui.actionGrid,
        &QAction::toggled,
        ui.networkView,
        &NetworkView::setGridEnabled);
    connect(
        ui.actionSnapToGrid,
        &QAction::toggled,
        ui.networkView,
        &NetworkView::setSnapToGridEnabled);
    connect(ui.networkView, &NetworkView::globalTickChanged, ui.tickNumberDisplay, [this]() {
        ui.tickNumberDisplay->display(QString::number(global_tick));
    });
    ui.simSpeedSpinBox->setDecimals(4);
    ui.simSpeedSpinBox->setSingleStep(0.001);
    connect(ui.simSpeedSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        settings.sim_speed = value;
    });
}

MainWindow::~MainWindow() {
}
