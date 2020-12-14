#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <stack>
#include "Core/Base.h"
#include "Command.h"

typedef std::stack<UniquePtr<Command>> CommandStack;

class CommandManager {
	CommandStack mUndoStack = CommandStack();;
	CommandStack mRedoStack;
public:
	void ExecuteCommand(UniquePtr<Command> command);
	void Undo();
	void Redo();
	void Clear();
};
#endif //COMMANDMANAGER_H