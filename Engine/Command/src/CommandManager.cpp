#include "Command/CommandManager.h"

CommandStack CommandManager::UndoStack;
CommandStack CommandManager::RedoStack;

void CommandManager::Init()
{
    UndoStack = CommandStack();
}

//template<typename T, typename...Args>
//void CommandManager::ExecuteA(T command, Args&&... args)
//{
//    RedoStack = CommandStack();
//    UndoStack.push(std::make_unique<T>(std::forward<Args>(args)...));
//}

void CommandManager::Execute(UniquePtr<Command> command)
{
    RedoStack = CommandStack();
    command->Execute();
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
        RedoStack.top().reset();
        RedoStack.pop();
    }
}

bool CommandManager::IsRedoStackEmpty()
{
    return RedoStack.empty();
}
