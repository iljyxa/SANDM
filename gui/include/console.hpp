#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <QKeyEvent>
#include <QTextCursor>
#include <QTextEdit>

class Console final : public QTextEdit {
    Q_OBJECT

public:
    explicit Console(QWidget *parent = nullptr) :
        QTextEdit(parent), input_start_pos_(-1) {
        setReadOnly(true);
    }

    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;

    QString GetInputString() {
        input_start_pos_ = textCursor().position();
        setReadOnly(false);
        moveCursor(QTextCursor::End);

        QEventLoop loop;
        connect(this, &Console::InputFinished, &loop, &QEventLoop::quit);
        loop.exec();

        setReadOnly(true);
        return entered_text_;
    }

signals:
    void InputFinished(const QString &text);

protected:
    void keyPressEvent(QKeyEvent *event) override {

        if (isReadOnly()) {
            QTextEdit::keyPressEvent(event);
            return;
        }

        QTextCursor cursor = textCursor();

        const int cursor_position = cursor.position();
        if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down
            || event->key() == Qt::Key_PageUp || event->key() == Qt::Key_PageDown) {

            return;
        }

        if (cursor_position <= input_start_pos_) {
            cursor.setPosition(input_start_pos_);
            setTextCursor(cursor);

            if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Left) {
                return;
            }

        }

        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            cursor.setPosition(input_start_pos_);
            cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
            entered_text_ = cursor.selectedText().trimmed(); // Получаем введенный текст

            // Испускаем сигнал о завершении ввода
            emit InputFinished(entered_text_);

            // Добавляем перенос строки
            moveCursor(QTextCursor::End); // Перемещаем курсор в конец
            insertPlainText("\n"); // Вставляем новую строку
            return;
        }

        QTextEdit::keyPressEvent(event);
    }

private:
    int input_start_pos_;
    QString entered_text_;
};

#endif
