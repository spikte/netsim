#ifndef GUI_DIALOG_HPP
#define GUI_DIALOG_HPP

#include "../include/sim/interface.hpp"
#include "validator.hpp"
#include "../include/sim/device.hpp"
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGraphicsItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QComboBox>

struct InterfaceConfigDialog : public QDialog {
    Q_OBJECT
  public:
    explicit InterfaceConfigDialog(QWidget* parent = nullptr);
    explicit InterfaceConfigDialog(const Interface* interface, QWidget* parent = nullptr);
    QComboBox* typeEdit;
};

#endif
