#ifndef GUI_TERMINAL_WIDGET_HPP
#define GUI_TERMINAL_WIDGET_HPP

#include "../include/sim/routing_processor.hpp"
#include "computer_item.hpp"
#include "router_item.hpp"
#include "shell.hpp"
#include "switch_item.hpp"
#include <QFontDatabase>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTimer>
#include <QWidget>
#include <typeinfo>

// Assume the user pass a valid array, i.e every row has the same number of column
using StringTable = std::vector<std::vector<std::string>>;
inline std::size_t stringTableRow(const StringTable& table) {
    return table.size();
}
inline std::size_t stringTableCol(const StringTable& table) {
    if (table.empty())
        return 0;
    return table[0].size();
}
inline std::string stringTableGet(const StringTable& table, std::size_t i, std::size_t j) {
    assert(i < stringTableRow(table) && j < stringTableCol(table));
    return table[i][j];
}
std::vector<int> stringTableMaxLen(const StringTable& table);

class AsyncJob : public QObject {
    Q_OBJECT
  public:
    virtual void abort() = 0;
  signals:
    void finished();
  public slots:
    virtual void onTick() = 0;
};

struct RunningJobState {
    AsyncJob* job;
    template <typename T>
    void keyPressEvent(TerminalWidget<T>& terminal, QKeyEvent* event) {
        if (event->key() == Qt::Key_C && event->modifiers() & Qt::ControlModifier) {
            terminal.appendMessage("^C");
            job->abort();
        }
    }
};
struct InputState {
    template <typename T>
    void keyPressEvent(TerminalWidget<T>& terminal, QKeyEvent* event) {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            terminal.historyPosition         = -1;
            QString command                  = terminal.document()->toPlainText().mid(terminal.promptPosition);
            bool onlySpaces                  = !command.isEmpty() && command.trimmed().isEmpty();
            HandlerReturnType handler_return = terminal.shell.execute_command(command.toStdString());
            if (!onlySpaces && handler_return == HandlerReturnType::NotFound)
                terminal.appendMessage("Command not found.");
            if(handler_return != HandlerReturnType::Async)
                terminal.appendPrompt();
        } else if (event->key() == Qt::Key_Up) {
            if (terminal.historyPosition == -1)
                terminal.historyPosition = 0;
            else if (terminal.historyPosition < terminal.shell.history.size() - 1)
                terminal.historyPosition++;
            terminal.setHistoryInput();
        } else if (event->key() == Qt::Key_Down) {
            if (terminal.historyPosition > -1)
                terminal.historyPosition--;
            terminal.setHistoryInput();
        } else if (!(event->key() == Qt::Key_Backspace && terminal.textCursor().position() <= terminal.promptPosition))
            terminal.qPlainTextEditKeyPressEvent(event);
        if (terminal.textCursor().position() < terminal.promptPosition)
            terminal.setCursorPositionAtPrompt();
    }
};

struct TerminalSink {
    virtual void appendMessage(std::string message) = 0;
    virtual void appendPrompt()                     = 0;
    virtual ~TerminalSink()                         = default;
};
;

using TerminalWidgetState = std::variant<InputState, RunningJobState>;
template <typename T>
struct TerminalWidget : public QPlainTextEdit, public TerminalSink {
    T* item;
    int64_t historyPosition;
    Shell<TerminalWidget<T>> shell;
    QString prompt;
    int promptPosition;
    TerminalWidgetState state;
    TerminalWidget(T* item = nullptr, QWidget* parent = nullptr) : item(item), historyPosition(-1), QPlainTextEdit(parent), state(InputState()) {
        setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
        QFont font = this->font();
        font.setPointSize(16);
        setFont(font);
        setStyleSheet(
            "QPlainTextEdit {"
            "    background-color: #0d1117;"
            "    color: #c9d1d9;"
            "    selection-background-color: #264f78;"
            "    selection-color: #ffffff;"
            "    border: none;"
            "}");
        shell.device = this;
        clear();
        if (item && item->device)
            prompt = QString::fromStdString(std::format("{}> ", item->device->name));
        else
            prompt = QString::fromStdString("Device> ");
        appendPlainText(prompt);
        promptPosition = document()->characterCount() - 1;
        setCursorPosition(promptPosition);
    }

    void setCursorPosition(int position) {
        QTextCursor cursor = textCursor();
        cursor.setPosition(position);
        setTextCursor(cursor);
    }
    void setCursorPositionAtPrompt() {
        setCursorPosition(promptPosition);
    }
    void appendPrompt() {
        appendPlainText(prompt);
        promptPosition = document()->characterCount() - 1;
        setCursorPosition(promptPosition);
    }
    void appendMessage(std::string message) {
        appendPlainText(QString::fromStdString(message));
    }
    void appendTable(const StringTable& table, bool header = false) {
        if (table.empty())
            return;
        std::vector<int> maxLen = stringTableMaxLen(table);
        QString text;
        for (std::size_t i = 0; i < stringTableRow(table); ++i) {
            for (std::size_t j = 0; j < stringTableCol(table); ++j) {
                QString value = QString::fromStdString(stringTableGet(table, i, j));
                text += value.leftJustified(maxLen[j]);
                if (j + 1 < stringTableCol(table))
                    text += "  ";
            }
            text += "\n";

            if (i == 0 && header) {
                for (std::size_t j = 0; j < stringTableCol(table); ++j) {
                    text += QString(maxLen[j], '-');
                    if (j + 1 < stringTableCol(table))
                        text += "  ";
                }
                text += "\n";
            }
        }
        appendPlainText(text);
    }
    void setHistoryInput() {
        QTextCursor cursor = textCursor();
        cursor.setPosition(promptPosition);
        cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        if (historyPosition == -1 || historyPosition >= shell.history.size()) {
            cursor.removeSelectedText();
            setTextCursor(cursor);
            return;
        }
        QString history = QString::fromStdString(shell.history[shell.history.size() - 1 - historyPosition]);
        cursor.insertText(history);
        setTextCursor(cursor);
    }
    // TODO: there must a better way to do this
    void qPlainTextEditKeyPressEvent(QKeyEvent* event) {
        QPlainTextEdit::keyPressEvent(event);
    }
    void keyPressEvent(QKeyEvent* event) override {
        std::visit(
            [this, event](auto& state) {
                return state.keyPressEvent(*this, event);
            },
            state);
    }
    void wheelEvent(QWheelEvent* event) override {
        if (event->modifiers() & Qt::ControlModifier) {
            QFont font = this->font();
            int size   = font.pointSize();

            if (event->angleDelta().y() > 0)
                size++;
            else
                size--;

            size = std::clamp(size, 8, 32);

            font.setPointSize(size);
            setFont(font);

            event->accept();
            return;
        }

        QPlainTextEdit::wheelEvent(event);
    }
};

template <typename T>
using TerminalType = TerminalWidget<T>*;
using ArgType      = const std::vector<std::string>&;

#endif
