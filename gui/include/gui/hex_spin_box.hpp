#ifndef HEX_SPIN_BOX_HPP
#define HEX_SPIN_BOX_HPP

#include "gui/spin_box_hoverable.hpp"

/**
 * @class HexSpinBox
 * @brief Класс HexSpinBox расширяет функциональность QSpinBox для работы с шестнадцатеричными числами.
 *
 * Данный виджет предоставляет возможность ввода и отображения чисел в шестнадцатеричной системе
 * счисления, выполняя преобразования между строковым представлением и числами.
 *
 * Основные возможности HexSpinBox включают:
 * - Автоматическое преобразование введенного пользователем текста в числовое значение.
 * - Отображение чисел в формате шестнадцатеричных значений.
 */
class HexSpinBox final : public SpinBoxHoverable {
    Q_OBJECT
public:
    explicit HexSpinBox(QWidget *parent = nullptr) :
        SpinBoxHoverable(parent) {
        setPrefix("0x");
        setDisplayIntegerBase(16);
        setRange(INT_MIN, INT_MAX);
    }

    [[nodiscard]] unsigned int HexValue() const {
        return CastUnsignedInt(value());
    }

    void SetHexValue(const unsigned int value) {
        setValue(CastInt(value));
    }

protected:
    [[nodiscard]] QString textFromValue(const int value) const override {
        return QString("%1").arg(CastUnsignedInt(value), 8, 16, QLatin1Char('0')).toUpper();
    }

    [[nodiscard]] int valueFromText(const QString &text) const override {
        QString copy = text;
        if (copy.startsWith("0x"))
            copy.remove(0, 2);
        return CastInt(copy.toUInt(nullptr, 16));
    }

    QValidator::State validate(QString &input, int &pos) const override {
        QString copy(input);

        if (copy.startsWith("0x"))
            copy.remove(0, 2);

        pos -= copy.size() - copy.trimmed().size(); // NOLINT(*-narrowing-conversions)
        copy = copy.trimmed();
        if (copy.isEmpty())
            return QValidator::Intermediate;

        const QRegularExpression hex_regex("^[0-9A-Fa-f]{0,8}$");
        if (!hex_regex.match(copy).hasMatch())
            return QValidator::Invalid;

        input = QString("0x") + copy.toUpper();
        return QValidator::Acceptable;
    }

private:
    [[nodiscard]] static unsigned int CastUnsignedInt(int i) {
        return *reinterpret_cast<unsigned int*>(&i);
    }

    [[nodiscard]] static int CastInt(unsigned int u) {
        return *reinterpret_cast<int*>(&u);
    }
};

#endif