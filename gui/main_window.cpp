#include "main_window.hpp"

#include <QFile>
#include <QResizeEvent>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), toolGroup(this) {
    ui.setupUi(this);
    setupTools();
    QFile file(":/style.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        setStyleSheet(QString::fromUtf8(file.readAll()));
    }
    connect(&toolGroup,
            &QButtonGroup::idClicked,
            this,
            [this](int id) {
                ui.networkView->setTool(
                    static_cast<NetworkView::Tool>(id));
            });
    connect(ui.actionGrid,
            &QAction::toggled,
            ui.networkView,
            &NetworkView::setGridEnabled);
    connect(ui.actionSnapToGrid,
            &QAction::toggled,
            ui.networkView,
            &NetworkView::setSnapToGridEnabled);
    connect(ui.actionVlanViz,
            &QAction::toggled,
            ui.networkView,
            &NetworkView::setVlanVizEnabled);
    connect(ui.networkView,
            &NetworkView::onTick,
            ui.tickNumberDisplay,
            [this]() {
                ui.tickNumberDisplay->display(
                    QString::number(global_tick));
            });
    ui.simSpeedSpinBox->setDecimals(4);
    ui.simSpeedSpinBox->setSingleStep(0.001);
    connect(ui.simSpeedSpinBox,
            &QDoubleSpinBox::valueChanged,
            this,
            [this](double value) {
                settings.sim_speed = value;
            });
}

void MainWindow::setupTools() {
    toolGroup.addButton(ui.selectButton, static_cast<int>(NetworkView::Tool::Select));
    toolGroup.addButton(ui.switchButton, static_cast<int>(NetworkView::Tool::Switch));
    toolGroup.addButton(ui.computerButton, static_cast<int>(NetworkView::Tool::Computer));
    toolGroup.addButton(ui.linkButton, static_cast<int>(NetworkView::Tool::Link));
    toolGroup.addButton(ui.deleteButton, static_cast<int>(NetworkView::Tool::Delete));
    toolGroup.addButton(ui.routerButton, static_cast<int>(NetworkView::Tool::Router));
    QColor tint("#FAFAFA");
    ui.switchButton->setIcon(QIcon(assets.iconSwitch(tint)));
    ui.routerButton->setIcon(QIcon(assets.iconRouter(tint)));
    ui.computerButton->setIcon(QIcon(assets.iconComputer(tint)));
    ui.linkButton->setIcon(QIcon(assets.iconLink(tint)));
    ui.selectButton->setIcon(QIcon(assets.iconSelect(tint)));
    ui.deleteButton->setIcon(QIcon(assets.iconDelete(tint)));
    ui.switchButton->setIconSize(QSize(48, 48));
    ui.routerButton->setIconSize(QSize(48, 48));
    ui.computerButton->setIconSize(QSize(48, 48));
    ui.linkButton->setIconSize(QSize(48, 48));
    ui.selectButton->setIconSize(QSize(48, 48));
    ui.deleteButton->setIconSize(QSize(48, 48));

    toolGroup.setExclusive(true);
    ui.verticalLayout_4->removeWidget(ui.toolsWidget);
    ui.toolsWidget->setParent(ui.networkContainer);
    ui.networkView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui.toolsWidget->adjustSize();
    positionTools();
}

void MainWindow::positionTools() {
    const int x = (ui.networkContainer->width() - ui.toolsWidget->width()) / 2;
    const int y = ui.networkContainer->height() - ui.toolsWidget->height() - 50;
    ui.toolsWidget->move(x, y);
    ui.toolsWidget->raise();
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    positionTools();
}
