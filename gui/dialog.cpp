#include "dialog.hpp"

/* == InterfaceConfigDialog == */
InterfaceConfigDialog::InterfaceConfigDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Configure Interface");

    nameEdit = new QLineEdit(this);
    addressEdit = new QSpinBox(this);

    mtuEdit = new QSpinBox(this);
    mtuEdit->setRange(576, 65535);
    mtuEdit->setValue(1500);

    bufferSizeEdit = new QSpinBox(this);
    bufferSizeEdit->setRange(1, 1000000);
    bufferSizeEdit->setValue(100);

    auto* layout = new QFormLayout(this);
    layout->addRow("Name", nameEdit);
    layout->addRow("MAC Address:", addressEdit);
    layout->addRow("MTU:", mtuEdit);
    layout->addRow("Buffer Size:", bufferSizeEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
InterfaceConfigDialog::InterfaceConfigDialog(const Interface* interface, QWidget* parent): InterfaceConfigDialog(parent) {
    nameEdit->setText(QString::fromStdString(interface->name));
    addressEdit->setValue(interface->address);
    mtuEdit->setValue(interface->mtu);
    bufferSizeEdit->setValue(interface->buffer_size);
}
QString InterfaceConfigDialog::name() const {
    return nameEdit->text();
}
MACAddress InterfaceConfigDialog::address() const {
    return MACAddress(static_cast<uint64_t>(addressEdit->value()));
}
uint32_t InterfaceConfigDialog::mtu() const {
    return static_cast<uint32_t>(mtuEdit->value());
}
size_t InterfaceConfigDialog::bufferSize() const {
    return static_cast<size_t>(bufferSizeEdit->value());
}
/* == FrameCreationDialog == */
FrameCreationDialog::FrameCreationDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Configure Interface");

    srcEdit       = new QSpinBox(this);
    dstEdit       = new QSpinBox(this);
    hasTagEdit    = new QCheckBox(this);
    tagEdit       = new QSpinBox(this);
    ethertypeEdit = new QSpinBox(this);
    contentEdit   = new QPlainTextEdit(this);

    auto* layout = new QFormLayout(this);
    layout->addRow("Source", srcEdit);
    layout->addRow("Destination", dstEdit);
    layout->addRow("802.1Q tag", hasTagEdit);
    layout->addRow("Tag (PCP|DEI|VID)", tagEdit);
    layout->addRow("Ether Type", ethertypeEdit);
    layout->addRow("Content", contentEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
MACAddress FrameCreationDialog::src() const {
    return MACAddress(static_cast<uint64_t>(srcEdit->value()));
}
MACAddress FrameCreationDialog::dst() const {
    return MACAddress(static_cast<uint64_t>(dstEdit->value()));
}
bool FrameCreationDialog::hasTag() const {
    return hasTagEdit->isChecked();
}
uint16_t FrameCreationDialog::tag() const {
    return static_cast<uint16_t>(tagEdit->value());
}
uint16_t FrameCreationDialog::ethertype() const {
    return static_cast<uint16_t>(ethertypeEdit->value());
}
