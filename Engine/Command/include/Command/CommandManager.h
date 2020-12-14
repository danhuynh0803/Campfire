#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <stack>
#include "Core/Base.h"
#include "Command/Command.h"

typedef std::stack<UniquePtr<Command>> CommandStack;

static class CommandManager {
	static CommandStack mUndoStack;
	static CommandStack mRedoStack;
public:
	static void ExecuteCommand(UniquePtr<Command> command);
	static void Init();
	static void Undo();
	static void Redo();
	static void Clear();
};
#endif //COMMANDMANAGER_H