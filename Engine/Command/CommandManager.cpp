#include "CommandManager.h"

void CommandManager::Init()
{
    mUndoStack = CommandStack();
}

void CommandManager::ExecuteCommand(SharedPtr<Command> command)
{
    mRedoStack = CommandStack();
    command->Execute();
    mUndoStack.push(command);
}

void CommandManager::Undo()
{
    if (mUndoStack.size() <= 0) return;
    mUndoStack.top()->Undo();
    mRedoStack.push(mUndoStack.top());
    mUndoStack.pop();
}

void CommandManager::Redo()
{
    if (mRedoStack.size() <= 0) return;
    mRedoStack.top()->Redo();
    mUndoStack.push(mRedoStack.top());
    mRedoStack.pop();
}