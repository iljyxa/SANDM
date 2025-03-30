#ifndef SYNTAX_HIGHLIGHTER_HPP
#define SYNTAX_HIGHLIGHTER_HPP

#include <QRegularExpression>
#include <QSyntaxHighlighter>

class Highlighter final : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit Highlighter(QTextDocument *parent = nullptr) :
        QSyntaxHighlighter(parent) {

        auto comment = QColor(122, 126, 133);
        auto number = QColor(39, 171, 183);
        auto keyword = QColor(206, 141, 104);
        auto type_modifier = QColor(181, 182, 227);
        auto arg_modifier = QColor(147, 115, 165);
        auto other = QColor(188, 190, 196);

        // Команды
        QTextCharFormat keyword_format;
        keyword_format.setForeground(keyword);
        const QString keyword_patterns[] = {
                QStringLiteral("\\bNOP\\b"), QStringLiteral("\\bADD\\b"), QStringLiteral("\\bSUB\\b"),
                QStringLiteral("\\bMUL\\b"), QStringLiteral("\\bDIV\\b"), QStringLiteral("\\bMOD\\b"),
                QStringLiteral("\\bJMP\\b"), QStringLiteral("\\bJNZ\\b"), QStringLiteral("\\bJGZ\\b"),
                QStringLiteral("\\bSET\\b"), QStringLiteral("\\bSAVE\\b"), QStringLiteral("\\bLOAD\\b"),
                QStringLiteral("\\bREAD\\b"), QStringLiteral("\\bWRITE\\b"), QStringLiteral("\\bPAGE\\b")
        };
        for (const QString &pattern: keyword_patterns) {
            highlighting_rules_.append({QRegularExpression(pattern), keyword_format});
        }

        // Модификаторы типов
        QTextCharFormat type_modificator_format;
        type_modificator_format.setForeground(type_modifier);
        const QString types_patterns[] = {QStringLiteral("\\bC\\b"), QStringLiteral("\\bW\\b"),
                                          QStringLiteral("\\bSW\\b"), QStringLiteral("\\bR\\b")};
        for (const QString &pattern: types_patterns) {
            highlighting_rules_.append({QRegularExpression(pattern), type_modificator_format});
        }

        // Модификаторы аргумента
        QTextCharFormat argument_modificator_format;
        argument_modificator_format.setForeground(arg_modifier);
        highlighting_rules_.append({QRegularExpression("\\B&\\B"), argument_modificator_format});

        // Метки
        QTextCharFormat label_format;
        label_format.setForeground(other);
        highlighting_rules_.append({QRegularExpression(QStringLiteral("^[\\s]*([a-zA-Z_0-9]*:)\\B")), label_format});

        // Числа
        QTextCharFormat number_format;
        number_format.setForeground(number);
        highlighting_rules_.append(
        {QRegularExpression(QStringLiteral("\\b(?:\\d+(?:\\.\\d*)?|0x[0-9A-Fa-f]+|0b[01]+)\\b")),
         number_format});

        // Комментарии
        QTextCharFormat comment_format;
        comment_format.setForeground(comment);
        highlighting_rules_.append({QRegularExpression(";[^\n]*"), comment_format});

    }

protected:
    void highlightBlock(const QString &text) override {
        const QString text_upper = text.toUpper();
        for (const auto &[pattern, format]: std::as_const(highlighting_rules_)) {
            QRegularExpressionMatchIterator match_iterator = pattern.globalMatch(text_upper);
            while (match_iterator.hasNext()) {
                QRegularExpressionMatch match = match_iterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), format);
            }
        }
    }

private:
    QList<std::pair<QRegularExpression, QTextCharFormat>> highlighting_rules_;
};

#endif
