#ifndef EDITOR_H
#define EDITOR_H

#include <QPlainTextEdit>
#include <QVector>
#include <memory>

class UndoRedoStack;
class SyntaxHighlighter;
class QResizeEvent;

/**
 * @brief Custom text editor widget with advanced features
 *
 * Extends QPlainTextEdit with line numbers, syntax highlighting,
 * undo/redo support, and other professional features.
 */
class Editor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor();

    // File operations
    void setFileName(const QString &fileName);
    QString fileName() const { return currentFileName; }
    QString fileExtension() const;
    bool isModified() const;
    void setModified(bool modified);

    // Text operations
    int lineCount() const;
    int currentLineNumber() const;
    int currentColumnNumber() const;
    QString selectedText() const;
    void selectAll();
    void selectLine();
    void selectWord();

    // Line operations
    void deleteLine();
    void duplicateLine();
    void moveLineUp();
    void moveLineDown();
    void insertLineAfter();
    void insertLineBefore();
    void joinWithNextLine();

    // Undo/Redo
    void undo();
    void redo();
    UndoRedoStack *getUndoRedoStack() const { return undoRedoStack.get(); }

    // Display options
    void setShowLineNumbers(bool show);
    bool showLineNumbers() const { return displayLineNumbers; }
    void setWordWrapMode(bool wrap);
    void setFontSize(int size);
    int fontSize() const { return currentFontSize; }

    // Syntax highlighting
    void setSyntaxHighlighting(bool enabled);
    bool syntaxHighlightingEnabled() const { return highlightingEnabled; }
    void updateSyntaxHighlighting();

    // Search highlighting
    void highlightOccurrences(const QString &text);
    void clearHighlights();

    // Getters
    QPlainTextEdit::LineWrapMode wordWrapMode() const;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);
    void onBlockCountChanged(int newBlockCount);
    void onCursorPositionChanged();

private:
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth() const;
    QString getLineText(int lineNumber) const;

    // UI Components
    class LineNumberArea;
    std::unique_ptr<LineNumberArea> lineNumberArea;

    // Managers
    std::unique_ptr<UndoRedoStack> undoRedoStack;
    std::unique_ptr<SyntaxHighlighter> syntaxHighlighter;

    // State
    QString currentFileName;
    int currentFontSize;
    bool displayLineNumbers;
    bool highlightingEnabled;

    friend class LineNumberArea;
};

#endif // EDITOR_H
