#include "Command/CommandManager.h"

CommandStack CommandManager::UndoStack;
CommandStack CommandManager::RedoStack;

void CommandManager::Init()
{
    UndoStack = CommandStack();
}

void CommandManager::ExecuteCommand(UniquePtr<Command> command)
{
    RedoStack = CommandStack();
    //design pattern wants us to invoke the change here
    command->Execute();
    //mUndoStack.push(command)?;
    UndoStack.push(std::move(command));
}

void CommandManager::Undo()
{
    if (UndoStack.size() <= 0) return;

    UndoStack.top()->Undo();
    RedoStack.push(std::move(UndoStack.top()));
    UndoStack.pop();
}

void CommandManager::Redo()
{
    if (RedoStack.size() <= 0) return;
    RedoStack.top()->Redo();
    UndoStack.push(std::move(RedoStack.top()));
    RedoStack.pop();
}

void CommandManager::Clear()
{
    while (!UndoStack.empty())
    {
        UndoStack.top().reset();
        UndoStack.pop();
    }
    while (!RedoStack.empty())
    {
        UndoStack.top().reset();
        RedoStack.pop();
    }
}

bool CommandManager::IsRedoStackEmpty()
{
    return RedoStack.empty();
}
