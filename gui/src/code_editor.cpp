#include "../../gui/include/code_editor.hpp"

#include <QPainterPath>
#include <QThreadPool>

CodeEditor::CodeEditor(Assembler& assembler, QWidget* parent) :
    QPlainTextEdit(parent),
    assembler_(assembler),
    hovered_line_(0) {

    // ReSharper disable once CppDFAMemoryLeak
    line_number_area_ = new LineNumberArea(this);
    line_number_area_->installEventFilter(this);
    line_number_area_->setMouseTracking(true);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::UpdateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::UpdateLineNumberArea);

    UpdateLineNumberAreaWidth(0);
    this->setFont(QFont("Droid Sans Mono", 13));

    QPalette palette = this->palette();
    palette.setColor(QPalette::Text, QColor(0xd4d4d4));
    this->setPalette(palette);

    highlighter_ = new Highlighter(document());

    check_timer_ = new QTimer(this);
    check_timer_->setInterval(1500);
    check_timer_->setSingleShot(true);

    connect(this, &CodeEditor::textChanged, this, &CodeEditor::OnTextChanged);
    connect(check_timer_, &QTimer::timeout, this, &CodeEditor::PerformChecks);
}

int CodeEditor::LineNumberAreaWidth() const {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    space += 10; // 10 пикселей для кружка

    return space;
}

void CodeEditor::UpdateLineNumberAreaWidth(int /* newBlockCount */) {
    setViewportMargins(LineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::UpdateLineNumberArea(const QRect& rect, const int dy) {
    if (dy)
        line_number_area_->scroll(0, dy);
    else
        line_number_area_->update(0, rect.y(), line_number_area_->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        UpdateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent* event) {
    QPlainTextEdit::resizeEvent(event);

    const QRect cr = contentsRect();
    line_number_area_->setGeometry(QRect(cr.left(), cr.top(), LineNumberAreaWidth(), cr.height()));
}

void CodeEditor::LineNumberAreaPaintEvent(const QPaintEvent* event) const {
    QPainter painter(line_number_area_);
    painter.fillRect(event->rect(), Qt::lightGray);

    constexpr int arrow_size = 8;
    constexpr int circle_size = 12;
    const QFontMetrics fm = fontMetrics();
    const int font_height = fm.height();

    QTextBlock block = firstVisibleBlock();
    int block_number = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const int current_line = block_number + 1;

            // Рисуем желтую стрелку для подсвеченной строки
            if (highlighted_lines_.contains(current_line)) {
                painter.setBrush(QColor(255, 255, 0)); // Желтый цвет
                painter.setPen(Qt::black);

                const int arrow_x = line_number_area_->width() - arrow_size - 3;
                const int arrow_y = top + (font_height - arrow_size) / 2;

                QPolygonF arrow;
                arrow << QPointF(arrow_x, arrow_y);
                arrow << QPointF(arrow_x + arrow_size, arrow_y + arrow_size / 2);
                arrow << QPointF(arrow_x, arrow_y + arrow_size);
                painter.drawPolygon(arrow);
            }

            // Остальная логика отрисовки (точки останова, hover) без изменений
            if (breakpoints_.contains(current_line)) {
                painter.setBrush(QColor(200, 79, 79));
                painter.setPen(Qt::NoPen);

                const int circle_x = (line_number_area_->width() - circle_size) / 2;
                const int circle_y = top + (font_height - circle_size) / 2;

                painter.drawEllipse(circle_x, circle_y, circle_size, circle_size);
            } else if (hovered_line_ == current_line) {
                painter.setBrush(QColor(200, 79, 79, 128));
                painter.setPen(Qt::NoPen);

                const int circle_x = (line_number_area_->width() - circle_size) / 2;
                const int circle_y = top + (font_height - circle_size) / 2;

                painter.drawEllipse(circle_x, circle_y, circle_size, circle_size);
            }

            // Номер строки
            if (!breakpoints_.contains(current_line)) {
                QString number = QString::number(current_line);
                painter.setPen(Qt::black);
                painter.drawText(0, top, line_number_area_->width(), font_height,
                                 Qt::AlignRight, number);
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++block_number;
    }
}

int CodeEditor::LineNumberAtPosition(const QPoint& pos) const {
    // Получаем координату Y клика мыши
    const int y = pos.y();

    // Находим блок текста, соответствующий этой координате
    QTextBlock block = firstVisibleBlock();
    int block_number = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= y) {
        if (block.isVisible() && bottom >= y) {
            return block_number + 1; // Возвращаем номер строки (начиная с 1)
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++block_number;
    }

    return -1; // Если строка не найдена
}

void CodeEditor::ToggleBreakpoint(const int line_number) {
    if (breakpoints_.contains(line_number)) {
        breakpoints_.remove(line_number); // Удаляем точку останова
        emit BreakpointRemoved(line_number);
    } else {
        breakpoints_.insert(line_number); // Добавляем точку останова
        emit BreakpointAdded(line_number);
    }

    line_number_area_->update(); // Обновляем область с номерами строк
}

void CodeEditor::DrawWavyLine(QPainter& painter, const QPointF& start, const QPointF& end) {
    QPainterPath path;
    path.moveTo(start);

    qreal x = start.x();
    qreal y = start.y();
    bool up = true;

    while (x < end.x()) {
        constexpr qreal wave_length = 6.0;
        constexpr qreal wave_height = 2.0;
        x += wave_length / 2;
        y += up ? -wave_height : wave_height;
        path.lineTo(x, y);
        up = !up;
    }

    painter.drawPath(path);
}

bool CodeEditor::eventFilter(QObject* obj, QEvent* event) {
    if (obj == line_number_area_) {
        if (event->type() == QEvent::MouseButtonRelease) {
            const auto* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (const int block_number = LineNumberAtPosition(mouse_event->pos()); block_number != -1) {
                ToggleBreakpoint(block_number);
            }
            return true;
        }

        if (event->type() == QEvent::MouseMove) {
            const auto* mouse_event = dynamic_cast<QMouseEvent*>(event);

            if (const int block_number = LineNumberAtPosition(mouse_event->pos()); block_number != -1) {
                line_number_area_->setCursor(Qt::PointingHandCursor);
                if (hovered_line_ != block_number) {
                    hovered_line_ = block_number;
                    line_number_area_->update();
                }
            } else {
                line_number_area_->setCursor(Qt::ArrowCursor);
                if (hovered_line_ != -1) {
                    hovered_line_ = -1;
                    line_number_area_->update();
                }
            }
            return true;
        }

        if (event->type() == QEvent::Leave) {
            line_number_area_->setCursor(Qt::ArrowCursor);
            if (hovered_line_ != -1) {
                hovered_line_ = -1;
                line_number_area_->update();
            }
        }
    }
    return QPlainTextEdit::eventFilter(obj, event);
}

void CodeEditor::paintEvent(QPaintEvent* event) {
    QPlainTextEdit::paintEvent(event);

    QPainter painter(viewport());

    // Подсвечиваем строки кода голубым
    for (const auto line : highlighted_lines_) {
        if (QTextBlock block = document()->findBlockByNumber(line - 1); block.isValid()) {
            QRectF rect = blockBoundingGeometry(block).translated(contentOffset()).toRect();
            painter.fillRect(QRect(0, rect.y(), viewport()->width(), rect.height()),
                             QColor(173, 216, 230, 100)); // Полупрозрачный голубой
        }
    }

    // Отрисовка волнистых линий для ошибок (без изменений)
    painter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    for (const auto i : underlined_lines_) {
        if (QTextBlock block = document()->findBlockByNumber(i - 1); block.isValid()) {
            QRectF rect = blockBoundingGeometry(block).translated(contentOffset()).toRect();
            if (QString text = block.text(); !text.isEmpty()) {
                QFontMetrics fm(block.charFormat().font());
                const int text_width = fm.horizontalAdvance(text);
                QPointF start = rect.bottomLeft();
                QPointF end = rect.bottomLeft() + QPointF(text_width, 0);
                DrawWavyLine(painter, start, end);
            }
        }
    }
}

void CodeEditor::OnTextChanged() const {
    check_timer_->start();
}

void CodeEditor::PerformChecks() {
    QThreadPool::globalInstance()->start([this] {
        AnalyzeCode();
    });
}

void CodeEditor::AnalyzeCode() {
    underlined_lines_.clear();
    std::vector<std::string> errors;
    try {
        errors = assembler_.TestSource(toPlainText().toStdString());
    } catch (...) {
        // Не обрабатывается в данном случае
    }

    for (const auto& error : errors) {
        const auto line_numbers = FindErrorLineNumbers(QString::fromStdString(error));
        for (const auto line_number : line_numbers) {
            underlined_lines_.append(line_number);
        }
    }

    QMetaObject::invokeMethod(this, [this] {
        viewport()->update();
    }, Qt::QueuedConnection);
}

QVector<unsigned int> CodeEditor::FindErrorLineNumbers(const QString& input) {
    QVector<unsigned int> result;
    const QRegularExpression pattern(R"(Line\s*(\d+):)");

    QRegularExpressionMatchIterator it = pattern.globalMatch(input);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        bool ok;
        unsigned int number = match.captured(1).toUInt(&ok);
        if (ok) {
            result.append(number);
        }
    }

    return result;
}

void CodeEditor::HighlightLine(const unsigned int line_number) {
    highlighted_lines_.insert(line_number);
    line_number_area_->update();
    viewport()->update();
}

void CodeEditor::ClearHighlightedLines() {
    highlighted_lines_.clear();
    line_number_area_->update();
    viewport()->update();
}
