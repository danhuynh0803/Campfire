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
	//template<typename T, typename ...Args>
	//static void ExecuteA(T command, Args && ...args);
	static void Init();
	static void Execute(UniquePtr<Command> command);
	static void Undo();
	static void Redo();
	static void Clear();
	static bool IsRedoStackEmpty();
};
#endif //COMMANDMANAGER_H