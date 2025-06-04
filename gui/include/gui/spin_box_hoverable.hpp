
#ifndef QSPINBOXHOVERABLE_HPP
#define QSPINBOXHOVERABLE_HPP

#include <QSpinBox>

class SpinBoxHoverable : public QSpinBox {
    Q_OBJECT
public:
    SpinBoxHoverable() = default;
    explicit SpinBoxHoverable(QWidget* parent = nullptr) : QSpinBox(parent) {};

signals:
    void Hovered(bool state);

protected:
    void enterEvent(QEnterEvent* event) override {
        emit Hovered(true);
        QSpinBox::enterEvent(event);
    }

    void leaveEvent(QEvent* event) override {
        emit Hovered(false);
        QSpinBox::leaveEvent(event);
    }
};

#endif //QSPINBOXHOVERABLE_HPP
