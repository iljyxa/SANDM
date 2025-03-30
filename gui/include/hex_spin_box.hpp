#ifndef HEX_SPIN_BOX_HPP
#define HEX_SPIN_BOX_HPP
#include <QSpinBox>

class HexSpinBox final : public QSpinBox {
public:
    explicit HexSpinBox(QWidget *parent = nullptr) :
        QSpinBox(parent) {
        setPrefix("0x");
        setDisplayIntegerBase(16);
        setRange(INT_MIN, INT_MAX);
    }

    [[nodiscard]] unsigned int HexValue() const {
        return u(value());
    }

    void SetHexValue(const unsigned int value) {
        setValue(i(value));
    }

protected:
    [[nodiscard]] QString textFromValue(const int value) const override {
        return QString::number(u(value), 16).toUpper();
    }

    [[nodiscard]] int valueFromText(const QString &text) const override {
        return i(text.toUInt(nullptr, 16));
    }

    QValidator::State validate(QString &input, int &pos) const override {

        QString copy(input);

        if (copy.startsWith("0x"))
            copy.remove(0, 2);

        pos -= copy.size() - copy.trimmed().size();
        copy = copy.trimmed();
        if (copy.isEmpty())
            return QValidator::Intermediate;

        input = QString("0x") + copy.toUpper();
        bool okay;
        copy.toUInt(&okay, 16);

        if (!okay)
            return QValidator::Invalid;

        return QValidator::Acceptable;

    }

private:
    [[nodiscard]] static unsigned int u(int i) {
        return *reinterpret_cast<unsigned int *>(&i);
    }

    [[nodiscard]] static int i(unsigned int u) {
        return *reinterpret_cast<int *>(&u);
    }

};

#endif
