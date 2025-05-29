#ifndef COLORS_HPP
#define COLORS_HPP

#include <QApplication>
#include <QColor>
#include <QStyleHints>

class StyleColors {
public:
    static QColor Main() {
        return IsDarkTheme() ? QColor(43, 43, 43) : QColor(255, 255, 255);
    }

    static QColor Text() {
        return IsDarkTheme() ? QColor(220, 220, 220) : QColor(7, 7, 22);
    }

    static QColor TextHighlight() {
        return IsDarkTheme() ? QColor(33, 66, 131) : QColor(162, 205, 249);
    }

    static QColor Background() {
        return IsDarkTheme() ? QColor(31, 31, 31) : QColor(247, 248, 250);
    }

    static QColor LineNumberAreaBackground() {
        return IsDarkTheme() ? QColor(31, 31, 31) : QColor(255, 255, 255);
    }

    static QColor LineNumberAreaNumber() {
        return IsDarkTheme() ? QColor(75, 80, 89) : QColor(174, 178, 193);
    }

    static QColor LineNumberAreaSplitter() {
        return IsDarkTheme() ? QColor(49, 52, 56) : QColor(235, 236, 240);
    }

    static QColor CodeEditorComment() {
        return IsDarkTheme() ? QColor(122, 126, 133) : QColor(140, 140, 140);
    }

    static QColor CodeEditorNumber() {
        return IsDarkTheme() ? QColor(39, 171, 183) : QColor(23, 80, 235);
    }

    static QColor CodeEditorKeyword() {
        return IsDarkTheme() ? QColor(206, 141, 104) : QColor(0, 51, 179);
    }

    static QColor CodeEditorTypeModifier() {
        return IsDarkTheme() ? QColor(181, 182, 227) : QColor(0, 128, 128);
    }

    static QColor CodeEditorArgModifier() {
        return IsDarkTheme() ? QColor(147, 115, 165) : QColor(102, 14, 122);
    }

    static QColor CodeEditorChar() {
        return IsDarkTheme() ? QColor(0, 128, 0) : QColor(34, 139, 34);
    }

    static QColor CodeEditorOther() {
        return IsDarkTheme() ? QColor(188, 190, 196) : QColor(7, 7, 22);
    }

private:
    static bool IsDarkTheme() {
        return qApp && qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    }
};

#endif //COLORS_HPP
