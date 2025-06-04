#ifndef MEMORY_VIEW_HPP
#define MEMORY_VIEW_HPP

#include <QModelIndex>
#include <QMouseEvent>
#include <QPainter>
#include <QTableView>

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
            setEditTriggers(NoEditTriggers);;
        } else {
            setEditTriggers(DoubleClicked | EditKeyPressed);
        }
    }

signals:
    void CellHovered(int row, int column, common::Word value);

protected:
    void mouseMoveEvent(QMouseEvent* event) override {
        if (const QModelIndex index = indexAt(event->pos()); index.isValid()) {
            hovered_row_ = index.row();
            hovered_column_ = index.column();

            const common::Word value = model()->data(index).toString().toUInt(nullptr, 16);

            emit CellHovered(hovered_row_, hovered_column_, value);
        } else {
            hovered_row_ = -1;
            hovered_column_ = -1;
        }

        viewport()->update();
        QTableView::mouseMoveEvent(event);
    }

    void paintEvent(QPaintEvent* event) override {
        QTableView::paintEvent(event);

        if (hovered_row_ >= 0 && hovered_column_ >= 0) {
            QPainter painter(viewport());

            QRect row_rect = visualRect(model()->index(hovered_row_, 0));
            row_rect.setWidth(viewport()->width());
            painter.fillRect(row_rect, QColor(128, 128, 128, 50));

            QRect column_rect = visualRect(model()->index(0, hovered_column_));
            column_rect.setHeight(viewport()->height());
            painter.fillRect(column_rect, QColor(128, 128, 128, 50));
        }
    }

private:
    int hovered_row_;
    int hovered_column_;
};

#endif
