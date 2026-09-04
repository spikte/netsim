#ifndef GUI_TERMINAL_WIDGET_HPP
#define GUI_TERMINAL_WIDGET_HPP

#include "../include/computer.hpp"
#include "shell.hpp"
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QWidget>

// Assum the user pass a valid array, i.e every row has the same number of column
using StringTable = std::vector<std::vector<std::string>>;
inline size_t stringTableRow(const StringTable& table) {
    return table.size();
}
inline size_t stringTableCol(const StringTable& table) {
    if (table.empty())
        return 0;
    return table[0].size();
}
inline std::string stringTableGet(const StringTable& table, size_t i, size_t j) {
    assert(i < stringTableRow(table) && j < stringTableCol(table));
    return table[i][j];
}
std::vector<int> stringTableMaxLen(const StringTable& table);
template <typename T>
struct TerminalWidget : public QPlainTextEdit {
    Shell<TerminalWidget<T>> shell;
    T* device;
    QString prompt;
    int promptPosition;
    TerminalWidget(T* device = nullptr, QWidget* parent = nullptr): device(device), QPlainTextEdit(parent) {
        shell.device = this;
        clear();
        prompt = QString("Computer> ");
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
    void appendTable(const StringTable& table, bool header = false) {
        std::vector<int> maxLen;
        maxLen = stringTableMaxLen(table);
        QString text;
        for (int i = 0; i < stringTableRow(table); i++) {
            for (int j = 0; j < stringTableCol(table); j++) {
                QString value = QString::fromStdString(stringTableGet(table, i, j));
                text += value.leftJustified(maxLen[j], ' ');
                text += " ";
            }
            if (i == 0 && header) {
                for (int j = 0; j < stringTableCol(table); j++) {
                    text += QString(maxLen[j], '-');
                    text += " ";
                }
            }
            text += "\n";
        }
        appendPlainText(text);
    }
    void keyPressEvent(QKeyEvent* event) override {
        if (textCursor().position() < promptPosition)
            setCursorPositionAtPrompt();
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            QString command = document()->toPlainText().mid(promptPosition);
            shell.execute_command(command.toStdString());
            appendPrompt();
        }
        else
            QPlainTextEdit::keyPressEvent(event);
    }
};

extern CommandTrie<TerminalWidget<Computer>> computer_command_trie;
void build_computer_command_trie();

#endif
