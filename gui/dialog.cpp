#include "dialog.hpp"

/* == InterfaceConfigDialog == */
InterfaceConfigDialog::InterfaceConfigDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Add interface");

    typeEdit = new QComboBox(this);
    typeEdit->addItem("Console (RJ45)");
    typeEdit->addItem("Fast Ethernet (100Mbps)");
    typeEdit->addItem("Gigabit Ethernet (1Gps)");

    auto* layout = new QFormLayout(this);
    layout->addRow("Interface type", typeEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
