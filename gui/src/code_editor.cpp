
#include "gui/code_editor.hpp"

CodeEditor::CodeEditor(Assembler& assembler, QWidget* parent) :
    QPlainTextEdit(parent), assembler_(assembler), hovered_line_(0) {
    // ReSharper disable once CppDFAMemoryLeak
    line_number_area_ = new LineNumberArea(this);
    line_number_area_->installEventFilter(this);
    line_number_area_->setMouseTracking(true);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::UpdateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::UpdateLineNumberArea);
    connect(this, &CodeEditor::textChanged, this, &CodeEditor::OnTextChanged);

    UpdateLineNumberAreaWidth(0);

    setFont(QFont("Droid Sans Mono", 13));
    QTextOption option = document()->defaultTextOption();
    option.setTabStopDistance(40);
    document()->setDefaultTextOption(option);

    highlighter_ = new Highlighter(document());

    check_timer_ = new QTimer(this);
    check_timer_->setInterval(1500);
    check_timer_->setSingleShot(true);
    connect(check_timer_, &QTimer::timeout, this, &CodeEditor::PerformChecks);
}

int CodeEditor::LineNumberAreaWidth() const {
    int digits = 3;
    int max = qMax(1, blockCount());
    while (max >= 1000) {
        max /= 10;
        ++digits;
    }
    return 12 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
}

void CodeEditor::OnApplyTheme() {
    ApplyTheme();
    highlighter_->UpdateHighlightingRules();
    highlighter_->rehighlight();
}

void CodeEditor::UpdateLineNumberAreaWidth(int) {
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

void CodeEditor::DrawArrow(QPainter& painter, const int x, const int y) {
    constexpr int arrow_size = 10;
    const QPolygonF arrow{
        QPointF(x, y),
        QPointF(x + arrow_size, y + arrow_size / 2),
        QPointF(x, y + arrow_size)
    };
    painter.setBrush(QColor(255, 255, 0));
    painter.setPen(Qt::black);
    painter.drawPolygon(arrow);
}

void CodeEditor::DrawCircle(QPainter& painter, const int x, const int y, const bool transparent) {
    constexpr int circle_size = 14;
    QColor color(200, 79, 79);
    if (transparent)
        color.setAlpha(128);
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(x, y, circle_size, circle_size);
}

void CodeEditor::DrawLineNumber(QPainter& painter, const int top, const int font_height, const int width,
                                const int line_number) {
    const QString number = QString::number(line_number);
    painter.setPen(StyleColors::LineNumberAreaNumber());
    painter.drawText(0, top, width, font_height, Qt::AlignRight, number);
}

void CodeEditor::LineNumberAreaPaintEvent(const QPaintEvent* event) const {
    QPainter painter(line_number_area_);
    painter.fillRect(event->rect(), StyleColors::LineNumberAreaBackground());
    painter.setPen(StyleColors::LineNumberAreaSplitter());
    painter.drawLine(line_number_area_->width() - 1, 0, line_number_area_->width() - 1, line_number_area_->height());

    const QFontMetrics fm = fontMetrics();
    const int font_height = fm.height();
    QTextBlock block = firstVisibleBlock();
    int block_number = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const int current_line = block_number + 1;
            const int circle_x = (line_number_area_->width() - 14) / 2;
            const int circle_y = top + (font_height - 14) / 2;
            const int arrow_y = top + (font_height - 10) / 2;

            if (highlighted_lines_.contains(current_line))
                DrawArrow(painter, 0, arrow_y);

            if (breakpoints_.contains(current_line))
                DrawCircle(painter, circle_x, circle_y, false);
            else if (hovered_line_ == current_line)
                DrawCircle(painter, circle_x, circle_y, true);

            if (!breakpoints_.contains(current_line))
                DrawLineNumber(painter, top, font_height, line_number_area_->width(), current_line);
        }
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++block_number;
    }
}

int CodeEditor::LineNumberAtPosition(const QPoint& pos) const {
    const int y = pos.y();
    QTextBlock block = firstVisibleBlock();
    int block_number = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= y) {
        if (block.isVisible() && bottom >= y)
            return block_number + 1;
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++block_number;
    }
    return -1;
}

void CodeEditor::ToggleBreakpoint(const int line_number) {
    if (breakpoints_.contains(line_number)) {
        breakpoints_.remove(line_number);
        emit BreakpointRemoved(line_number);
    } else {
        breakpoints_.insert(line_number);
        emit BreakpointAdded(line_number);
    }
    line_number_area_->update();
}

void CodeEditor::DrawWavyLine(QPainter& painter, const QPointF& start, const QPointF& end) {
    QPainterPath path;
    path.moveTo(start);

    qreal x = start.x();
    qreal y = start.y();
    bool up = true;

    while (x < end.x()) {
        x += 3.0; // wave_length / 2
        y += up ? -2.0 : 2.0; // wave_height
        path.lineTo(x, y);
        up = !up;
    }
    painter.drawPath(path);
}

bool CodeEditor::eventFilter(QObject* obj, QEvent* event) {
    if (obj == line_number_area_) {
        if (const auto* mouse_event = dynamic_cast<QMouseEvent*>(event)) {
            if (event->type() == QEvent::MouseButtonRelease) {
                const int line = LineNumberAtPosition(mouse_event->pos());
                if (line != -1)
                    ToggleBreakpoint(line);
                return true;
            }
            if (event->type() == QEvent::MouseMove) {
                const int line = LineNumberAtPosition(mouse_event->pos());
                line_number_area_->setCursor(line != -1 ? Qt::PointingHandCursor : Qt::ArrowCursor);
                if (hovered_line_ != line) {
                    hovered_line_ = line;
                    line_number_area_->update();
                }
                return true;
            }
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

    // Highlight lines
    for (const unsigned int line : highlighted_lines_) {
        QTextBlock block = document()->findBlockByNumber(line - 1);
        if (block.isValid()) {
            QRectF rect = blockBoundingGeometry(block).translated(contentOffset()).toRect();
            painter.fillRect(QRect(0, rect.y(), viewport()->width(), rect.height()), QColor(173, 216, 230, 100));
        }
    }

    // Underline lines with errors
    painter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    for (const unsigned int line : underlined_lines_) {
        QTextBlock block = document()->findBlockByNumber(line - 1);
        if (block.isValid()) {
            QRectF rect = blockBoundingGeometry(block).translated(contentOffset()).toRect();
            QString text = block.text();
            if (!text.isEmpty()) {
                QFontMetrics fm(block.charFormat().font());
                const int width = fm.horizontalAdvance(text);
                DrawWavyLine(painter, rect.bottomLeft(), rect.bottomLeft() + QPointF(width, 0));
            }
        }
    }
}

void CodeEditor::OnTextChanged() const {
    check_timer_->start();
}

void CodeEditor::PerformChecks() {
    QThreadPool::globalInstance()->start([this] { AnalyzeCode(); });
}

void CodeEditor::AnalyzeCode() {
    underlined_lines_.clear();
    std::vector<std::string> errors;
    try {
        errors = assembler_.TestSource(toPlainText().toStdString());
    } catch (...) {
    }

    for (const auto& err : errors) {
        for (const int line : FindErrorLineNumbers(QString::fromStdString(err)))
            underlined_lines_.append(line);
    }

    QMetaObject::invokeMethod(this, [this] {
        viewport()->update(); }, Qt::QueuedConnection);
}

QVector<unsigned int> CodeEditor::FindErrorLineNumbers(const QString& input) {
    QVector<unsigned int> result;
    const QRegularExpression pattern(R"(Line\s*(\d+):)");
    auto it = pattern.globalMatch(input);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        bool ok;
        const unsigned int number = match.captured(1).toUInt(&ok);
        if (ok)
            result.append(number);
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

void CodeEditor::ApplyTheme() {
    QPalette palette = this->palette();
    palette.setColor(QPalette::Text, StyleColors::CodeEditorOther());
    setPalette(palette);
}

void CodeEditor::ScrollToLine(const int line_number) {
    const QTextBlock block = document()->findBlockByNumber(line_number - 1);
    if (block.isValid()) {
        setTextCursor(QTextCursor(block));
        centerCursor();
    }
}