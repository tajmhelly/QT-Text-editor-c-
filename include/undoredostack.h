#ifndef UNDOREDOSTACK_H
#define UNDOREDOSTACK_H

#include <QString>
#include <QStack>
#include <QVector>
#include <QObject>
#include <memory>

/**
 * @brief Represents a single undo/redo action
 */
class EditCommand
{
public:
    enum Type
    {
        Insert,
        Delete,
        Replace,
        Format
    };

    EditCommand(Type type, int position, const QString &text, int length = 0);

    Type type() const { return commandType; }
    int position() const { return commandPos; }
    QString text() const { return commandText; }
    int length() const { return commandLength; }
    long timestamp() const { return commandTimestamp; }

    void merge(const EditCommand &other);
    bool canMerge(const EditCommand &other) const;

private:
    Type commandType;
    int commandPos;
    QString commandText;
    int commandLength;
    long commandTimestamp;
};

/**
 * @brief Manages undo/redo operations for text editing
 *
 * Implements a stack-based undo/redo system with command merging
 * and efficient memory management.
 */
class UndoRedoStack : public QObject
{
    Q_OBJECT

public:
    explicit UndoRedoStack(QObject *parent = nullptr);
    ~UndoRedoStack();

    // Stack operations
    void push(const EditCommand &command);
    EditCommand undo();
    EditCommand redo();
    void clear();

    // State queries
    bool canUndo() const { return !undoStack.isEmpty(); }
    bool canRedo() const { return !redoStack.isEmpty(); }
    int undoCount() const { return undoStack.size(); }
    int redoCount() const { return redoStack.size(); }

    // Configuration
    void setMaxStackSize(int maxSize) { maxStackSize = maxSize; }
    void setMergeTimeout(int ms) { mergeTimeout = ms; }
    void setMergeEnabled(bool enabled) { mergingEnabled = enabled; }

    // History inspection
    int getCommandCount() const { return commandCount; }
    QString getCommandDescription(int index) const;
    void printHistory() const;

signals:
    void undoAvailable(bool available);
    void redoAvailable(bool available);
    void stackChanged();

private:
    void trimStack();
    void updateSignals();
    QString describeCommand(const EditCommand &cmd) const;

    QStack<EditCommand> undoStack;
    QStack<EditCommand> redoStack;
    int maxStackSize;
    int mergeTimeout;
    bool mergingEnabled;
    long lastCommandTime;
    int commandCount;
};

#endif // UNDOREDOSTACK_H
