#include "undoredostack.h"
#include <QDateTime>
#include <QDebug>

EditCommand::EditCommand(Type type, int position, const QString &text, int length)
    : commandType(type), commandPos(position), commandText(text), commandLength(length), commandTimestamp(QDateTime::currentMSecsSinceEpoch())
{
}

void EditCommand::merge(const EditCommand &other)
{
    if (canMerge(other))
    {
        commandText += other.commandText;
    }
}

bool EditCommand::canMerge(const EditCommand &other) const
{
    return commandType == other.commandType &&
           commandPos + static_cast<int>(commandText.length()) == other.commandPos;
}

UndoRedoStack::UndoRedoStack(QObject *parent)
    : QObject(parent), maxStackSize(500), mergeTimeout(500), mergingEnabled(true), lastCommandTime(0), commandCount(0)
{
}

UndoRedoStack::~UndoRedoStack() = default;

void UndoRedoStack::push(const EditCommand &command)
{
    // Clear redo stack when new command is pushed
    redoStack.clear();

    // Try to merge with last command if enabled
    if (mergingEnabled && !undoStack.isEmpty())
    {
        long timeDiff = command.timestamp() - lastCommandTime;
        if (timeDiff < mergeTimeout)
        {
            if (undoStack.top().canMerge(command))
            {
                undoStack.top().merge(command);
                lastCommandTime = command.timestamp();
                updateSignals();
                return;
            }
        }
    }

    undoStack.push(command);
    lastCommandTime = command.timestamp();
    commandCount++;

    trimStack();
    updateSignals();
}

EditCommand UndoRedoStack::undo()
{
    if (!canUndo())
    {
        return EditCommand(EditCommand::Insert, 0, "");
    }

    EditCommand command = undoStack.pop();
    redoStack.push(command);

    updateSignals();
    return command;
}

EditCommand UndoRedoStack::redo()
{
    if (!canRedo())
    {
        return EditCommand(EditCommand::Insert, 0, "");
    }

    EditCommand command = redoStack.pop();
    undoStack.push(command);

    updateSignals();
    return command;
}

void UndoRedoStack::clear()
{
    undoStack.clear();
    redoStack.clear();
    lastCommandTime = 0;
    commandCount = 0;
    updateSignals();
}

QString UndoRedoStack::getCommandDescription(int index) const
{
    if (index < 0 || index >= undoStack.size())
    {
        return "";
    }

    EditCommand cmd = undoStack.at(undoStack.size() - 1 - index);
    return describeCommand(cmd);
}

void UndoRedoStack::printHistory() const
{
    qDebug() << "=== Undo History ===";
    for (int i = 0; i < undoStack.size(); ++i)
    {
        qDebug() << "Command" << i << ":" << describeCommand(undoStack.at(i));
    }
}

void UndoRedoStack::trimStack()
{
    while (undoStack.size() > maxStackSize)
    {
        undoStack.removeFirst();
    }
}

void UndoRedoStack::updateSignals()
{
    emit undoAvailable(canUndo());
    emit redoAvailable(canRedo());
    emit stackChanged();
}

QString UndoRedoStack::describeCommand(const EditCommand &cmd) const
{
    switch (cmd.type())
    {
    case EditCommand::Insert:
        return QString("Insert at pos %1: '%2'").arg(cmd.position()).arg(cmd.text());
    case EditCommand::Delete:
        return QString("Delete at pos %1: %2 chars").arg(cmd.position()).arg(cmd.length());
    case EditCommand::Replace:
        return QString("Replace at pos %1: '%2'").arg(cmd.position()).arg(cmd.text());
    case EditCommand::Format:
        return QString("Format at pos %1").arg(cmd.position());
    default:
        return "Unknown";
    }
}
