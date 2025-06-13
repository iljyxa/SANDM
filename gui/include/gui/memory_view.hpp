#ifndef MEMORY_VIEW_HPP
#define MEMORY_VIEW_HPP

#include <QModelIndex>
#include <QMouseEvent>
#include <QTableView>
#include <QPainter>

#include "core/common_definitions.hpp"

/**
 * @class MemoryView
 * @brief Класс для визуализации авбстрактной модели памяти
 */
class MemoryView final : public QTableView {
    Q_OBJECT

public:
    explicit MemoryView(QWidget* parent = nullptr) :
        QTableView(parent), hovered_row_(-1), hovered_column_(-1) {
        setMouseTracking(true);
        setSelectionMode(SingleSelection);
        setSelectionBehavior(SelectItems);
    }

    void SetReadOnly(const bool read_only) {
        if (read_only) {
            setEditTriggers(NoEditTriggers);
        } else {
            setEditTriggers(DoubleClicked | EditKeyPressed);
        }
    }

signals:
    void CellHovered(int row, int column, std::optional<snm::Word> value);

protected:
    void paintEvent(QPaintEvent* event) override {
        QTableView::paintEvent(event);  // Стандартная отрисовка таблицы

        // Если есть hover-ячейка, рисуем поверх неё полупрозрачный прямоугольник
        if (hovered_row_ >= 0 && hovered_column_ >= 0) {
            QPainter painter(viewport());
            const QRect rect = visualRect(model()->index(hovered_row_, hovered_column_));
            painter.fillRect(rect, QColor(128, 128, 128, 50));  // Полупрозрачный серый
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (const QModelIndex index = indexAt(event->pos()); index.isValid()) {
            hovered_row_ = index.row();
            hovered_column_ = index.column();

            const snm::Word value = model()->data(index).toString().toUInt(nullptr, 16);

            emit CellHovered(hovered_row_, hovered_column_, value);
        } else {
            hovered_row_ = -1;
            hovered_column_ = -1;
            emit CellHovered(hovered_row_, hovered_column_, std::nullopt);
        }

        viewport()->update();
        QTableView::mouseMoveEvent(event);
    }

    void leaveEvent(QEvent* event) override {
        hovered_row_ = -1;
        hovered_column_ = -1;
        emit CellHovered(hovered_row_, hovered_column_, std::nullopt);
        viewport()->update();
        QTableView::leaveEvent(event);
    }


private:
    int hovered_row_;
    int hovered_column_;
};

#endif