#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <stack>
#include "Core/Base.h"
#include "Command/Command.h"

typedef std::stack<UniquePtr<Command>> CommandStack;

static class CommandManager {
	static CommandStack UndoStack;
	static CommandStack RedoStack;
public:
	static void ExecuteCommand(UniquePtr<Command> command);
	static void Init();
	static void Undo();
	static void Redo();
	static void Clear();
	static bool IsRedoStackEmpty();
};
#endif //COMMANDMANAGER_H