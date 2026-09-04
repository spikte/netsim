#ifndef GUI_DIALOG_HPP
#define GUI_DIALOG_HPP

#include "../include/interface.hpp"
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGraphicsItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QStyle>
#include <QStyleOptionGraphicsItem>

struct InterfaceConfigDialog : public QDialog {
    Q_OBJECT
  public:
    explicit InterfaceConfigDialog(QWidget* parent = nullptr);
    explicit InterfaceConfigDialog(const Interface* interface, QWidget* parent = nullptr);
    QString name() const;
    MACAddress address() const;
    uint32_t mtu() const;
    size_t bufferSize() const;

  private:
    QLineEdit* nameEdit;
    QSpinBox* addressEdit;
    QSpinBox* mtuEdit;
    QSpinBox* bufferSizeEdit;
};

struct FrameCreationDialog : public QDialog {
    Q_OBJECT
  public:
    explicit FrameCreationDialog(QWidget* parent = nullptr);
    MACAddress src() const;
    MACAddress dst() const;
    bool hasTag() const;
    uint16_t tag() const;
    uint16_t ethertype() const;
    std::string content() const;

  private:
    QSpinBox* srcEdit;
    QSpinBox* dstEdit;
    QCheckBox* hasTagEdit;
    QSpinBox* tagEdit;
    QSpinBox* ethertypeEdit;
    QPlainTextEdit* contentEdit;
};

#endif
