#include "editor.h"
#include "undoredostack.h"
#include "syntaxhighlighter.h"

#include <QPainter>
#include <QTextEdit>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QScrollBar>
#include <QPlainTextEdit>
#include <QAbstractTextDocumentLayout>
#include <QTextBlock>
#include <QFont>
#include <QFontMetrics>
#include <QDebug>

/**
 * @brief Line number area widget
 */
class Editor::LineNumberArea : public QWidget
{
public:
    LineNumberArea(Editor *editor) : QWidget(editor), editor(editor) {}

    QSize sizeHint() const override
    {
        return QSize(editor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        editor->lineNumberAreaPaintEvent(event);
    }

private:
    Editor *editor;
};

Editor::Editor(QWidget *parent)
    : QPlainTextEdit(parent), lineNumberArea(std::make_unique<LineNumberArea>(this)), undoRedoStack(std::make_unique<UndoRedoStack>(this)), syntaxHighlighter(std::make_unique<SyntaxHighlighter>(document())), currentFileName("Untitled"), currentFontSize(12), displayLineNumbers(true), highlightingEnabled(true)
{
    // Setup font
    QFont font("Courier New", currentFontSize);
    font.setFixedPitch(true);
    setFont(font);

    // Setup connections
    connect(this, QOverload<int>::of(&QPlainTextEdit::blockCountChanged),
            this, &Editor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest,
            this, &Editor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged,
            this, &Editor::onCursorPositionChanged);
    connect(document(), &QTextDocument::modificationChanged,
            [this](bool changed)
            {
                if (changed)
                {
                    // Can emit signal here if needed
                }
            });

    // Setup tab width
    QFontMetrics fm(font);
    setTabStopDistance(4 * fm.horizontalAdvance(' '));

    // Initialize line number area
    updateLineNumberAreaWidth(0);

    // Set default syntax highlighting
    syntaxHighlighter->setLanguage(SyntaxHighlighter::PlainText);
    syntaxHighlighter->setTheme("Light");
}

Editor::~Editor() = default;

void Editor::setFileName(const QString &fileName)
{
    currentFileName = fileName;
    syntaxHighlighter->detectLanguageFromExtension(fileExtension());
}

QString Editor::fileExtension() const
{
    int lastDot = currentFileName.lastIndexOf('.');
    if (lastDot != -1)
    {
        return currentFileName.mid(lastDot + 1).toLower();
    }
    return "";
}

bool Editor::isModified() const
{
    return document()->isModified();
}

void Editor::setModified(bool modified)
{
    document()->setModified(modified);
}

int Editor::lineCount() const
{
    return blockCount();
}

int Editor::currentLineNumber() const
{
    return textCursor().blockNumber();
}

int Editor::currentColumnNumber() const
{
    return textCursor().positionInBlock();
}

QString Editor::selectedText() const
{
    return textCursor().selectedText();
}

void Editor::selectAll()
{
    QPlainTextEdit::selectAll();
}

void Editor::selectLine()
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    setTextCursor(cursor);
}

void Editor::selectWord()
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    setTextCursor(cursor);
}

void Editor::deleteLine()
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    cursor.removeSelectedText();
    cursor.deleteChar(); // Remove the newline
    setTextCursor(cursor);
}

void Editor::duplicateLine()
{
    QTextCursor cursor = textCursor();
    int lineNumber = cursor.blockNumber();
    QTextBlock block = document()->findBlockByNumber(lineNumber);

    QString lineText = block.text();
    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.insertText("\n" + lineText);
}

void Editor::moveLineUp()
{
    QTextCursor cursor = textCursor();
    int currentLine = cursor.blockNumber();

    if (currentLine > 0)
    {
        QTextBlock block = document()->findBlockByNumber(currentLine);
        QTextBlock prevBlock = document()->findBlockByNumber(currentLine - 1);

        QString currentText = block.text();
        QString prevText = prevBlock.text();

        cursor.select(QTextCursor::LineUnderCursor);
        cursor.removeSelectedText();

        cursor.movePosition(QTextCursor::Up);
        cursor.movePosition(QTextCursor::EndOfLine);
        cursor.insertText("\n" + currentText);
    }
}

void Editor::moveLineDown()
{
    QTextCursor cursor = textCursor();
    int currentLine = cursor.blockNumber();

    if (currentLine < blockCount() - 1)
    {
        QTextBlock block = document()->findBlockByNumber(currentLine);
        QTextBlock nextBlock = document()->findBlockByNumber(currentLine + 1);

        QString currentText = block.text();
        QString nextText = nextBlock.text();

        cursor.select(QTextCursor::LineUnderCursor);
        cursor.removeSelectedText();

        cursor.movePosition(QTextCursor::Down);
        cursor.movePosition(QTextCursor::EndOfLine);
        cursor.insertText("\n" + currentText);
    }
}

void Editor::insertLineAfter()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.insertText("\n");
    setTextCursor(cursor);
}

void Editor::insertLineBefore()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.insertText("\n");
    cursor.movePosition(QTextCursor::Up);
    setTextCursor(cursor);
}

void Editor::joinWithNextLine()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.deleteChar(); // Delete the newline
    cursor.insertText(" ");
}

void Editor::undo()
{
    QPlainTextEdit::undo();
}

void Editor::redo()
{
    QPlainTextEdit::redo();
}

void Editor::setShowLineNumbers(bool show)
{
    displayLineNumbers = show;
    lineNumberArea->setVisible(show);
    updateLineNumberAreaWidth(0);
}

void Editor::setWordWrapMode(bool wrap)
{
    if (wrap)
    {
        QPlainTextEdit::setWordWrapMode(QTextOption::WordWrap);
    }
    else
    {
        QPlainTextEdit::setWordWrapMode(QTextOption::NoWrap);
    }
}

void Editor::setFontSize(int size)
{
    if (size < 6 || size > 32)
        return;

    currentFontSize = size;
    QFont font = this->font();
    font.setPointSize(size);
    setFont(font);

    QFontMetrics fm(font);
    setTabStopDistance(4 * fm.horizontalAdvance(' '));

    updateLineNumberAreaWidth(0);
}

void Editor::setSyntaxHighlighting(bool enabled)
{
    highlightingEnabled = enabled;
    if (enabled)
    {
        syntaxHighlighter->setHighlightingEnabled(true);
    }
    else
    {
        syntaxHighlighter->setHighlightingEnabled(false);
    }
}

void Editor::updateSyntaxHighlighting()
{
    syntaxHighlighter->rehighlight();
}

void Editor::highlightOccurrences(const QString &text)
{
    // TODO: Implement occurrence highlighting
}

void Editor::clearHighlights()
{
    // TODO: Implement highlight clearing
}

void Editor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = QPlainTextEdit::contentsRect();
    lineNumberArea->setGeometry(
        cr.left(), cr.top(),
        lineNumberAreaWidth(), cr.height());
}

void Editor::keyPressEvent(QKeyEvent *event)
{
    // Handle special key combinations
    if (event->key() == Qt::Key_Tab)
    {
        // Insert spaces instead of tab
        textCursor().insertText("    ");
        return;
    }

    QPlainTextEdit::keyPressEvent(event);
}

void Editor::mousePressEvent(QMouseEvent *event)
{
    QPlainTextEdit::mousePressEvent(event);
}

void Editor::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        int delta = event->angleDelta().y();
        if (delta > 0)
        {
            setFontSize(currentFontSize + 1);
        }
        else if (delta < 0)
        {
            setFontSize(currentFontSize - 1);
        }
        event->accept();
    }
    else
    {
        QPlainTextEdit::wheelEvent(event);
    }
}

void Editor::updateLineNumberAreaWidth(int newBlockCount)
{
    if (displayLineNumbers)
    {
        setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
    }
    else
    {
        setViewportMargins(0, 0, 0, 0);
    }
}

void Editor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
    {
        lineNumberArea->scroll(0, dy);
    }
    else
    {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect()))
    {
        updateLineNumberAreaWidth(0);
    }
}

void Editor::onBlockCountChanged(int newBlockCount)
{
    updateLineNumberAreaWidth(newBlockCount);
}

void Editor::onCursorPositionChanged()
{
    // Could emit signal with current position
}

void Editor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    if (!displayLineNumbers)
        return;

    QPainter painter(lineNumberArea.get());
    painter.fillRect(event->rect(), QColor(240, 240, 240));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(100, 100, 100));
            painter.drawText(0, top, lineNumberArea->width() - 5,
                             fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int Editor::lineNumberAreaWidth() const
{
    if (!displayLineNumbers)
        return 0;

    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

QString Editor::getLineText(int lineNumber) const
{
    QTextBlock block = document()->findBlockByNumber(lineNumber);
    if (block.isValid())
    {
        return block.text();
    }
    return "";
}
