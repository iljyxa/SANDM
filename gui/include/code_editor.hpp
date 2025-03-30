#ifndef CODE_EDITOR_HPP
#define CODE_EDITOR_HPP

#include <QPainter>
#include <QPlainTextEdit>
#include <QTimer>
#include <iostream>

#include "syntax_highlighter.hpp"
#include "../../core/include/assembler.hpp"

class CodeEditor final : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit CodeEditor(Assembler& assembler, QWidget* parent = nullptr);
    CodeEditor(const CodeEditor&) = delete;
    CodeEditor& operator=(const CodeEditor&) = delete;

    void LineNumberAreaPaintEvent(const QPaintEvent* event) const;
    [[nodiscard]] int LineNumberAreaWidth() const;
    void HighlightLine(unsigned int line_number);
    void ClearHighlightedLines();
    void ScrollToLine(int line_number);

signals:
    void BreakpointAdded(unsigned int breakpoint);
    void BreakpointRemoved(unsigned int breakpoint);

public slots:
    void OnApplyTheme();

private slots:
    void UpdateLineNumberAreaWidth(int new_block_count);
    void UpdateLineNumberArea(const QRect& rect, int dy);
    void OnTextChanged() const;
    void PerformChecks();

protected:
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    QWidget* line_number_area_;
    Highlighter* highlighter_;
    QTimer* check_timer_;
    Assembler& assembler_;
    QVector<unsigned int> underlined_lines_;
    QSet<unsigned int> highlighted_lines_;
    QSet<unsigned int> breakpoints_;
    unsigned int hovered_line_;

    [[nodiscard]] int LineNumberAtPosition(const QPoint& pos) const;
    void ToggleBreakpoint(int line_number);
    static QVector<unsigned int> FindErrorLineNumbers(const QString& input);

    static void DrawWavyLine(QPainter& painter, const QPointF& start, const QPointF& end);
    void AnalyzeCode();
    void ApplyTheme();
};

class LineNumberArea final : public QWidget {
public:
    explicit LineNumberArea(CodeEditor* editor) :
        QWidget(editor), code_editor_(editor) {
    }

    [[nodiscard]] QSize sizeHint() const override {
        return {code_editor_->LineNumberAreaWidth(), 0};
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        code_editor_->LineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor* code_editor_;
};

#endif
