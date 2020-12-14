#include "Command/CommandManager.h"

CommandStack CommandManager::mUndoStack;
CommandStack CommandManager::mRedoStack;

void CommandManager::Init()
{
    mUndoStack = CommandStack();
}

void CommandManager::ExecuteCommand(UniquePtr<Command> command)
{
    mRedoStack = CommandStack();
    //design pattern wants us to invoke the change here
    command->Execute();
    //mUndoStack.push(command)?;
    mUndoStack.push(std::move(command));
}


void CommandManager::Undo()
{
    if (mUndoStack.size() <= 0) return;

    mUndoStack.top()->Undo();
    mRedoStack.push(std::move(mUndoStack.top()));
    mUndoStack.pop();
}

void CommandManager::Redo()
{
    if (mRedoStack.size() <= 0) return;
    mRedoStack.top()->Redo();
    mUndoStack.push(std::move(mRedoStack.top()));
    mRedoStack.pop();
}

void CommandManager::Clear()
{
    while (!mUndoStack.empty())
    {
        mUndoStack.top().reset();
        mUndoStack.pop();
    }
    while (!mRedoStack.empty())
    {
        mUndoStack.top().reset();
        mRedoStack.pop();
    }
}

bool CommandManager::IsRedoStackEmpty()
{
    return mRedoStack.empty();
}
