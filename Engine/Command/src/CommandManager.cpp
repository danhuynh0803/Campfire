#include "Command/CommandManager.h"

void CommandManager::Init()
{
    mUndoStack = CommandStack();
}

void CommandManager::ExecuteCommand(UniquePtr<Command> command)
{
    mRedoStack = CommandStack();
    command->Execute();
    //mUndoStack.push(command)?;
    mUndoStack.push(std::move(command));
}

void CommandManager::ExecuteCommand(const Command& command)
{
    mRedoStack = CommandStack(); 
    //Technically, the design pattern wants us to execute the command to apply the change here
    //For Imgui calls, this will be ignore

    //Having issue
    //mUndoStack.push(std::make_unique<Command>(command));
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
