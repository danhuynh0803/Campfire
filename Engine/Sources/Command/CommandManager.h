#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <stack>
#include "Core/Base.h"
#include "Command.h"

typedef std::stack<SharedPtr<Command>> CommandStack;

class CommandManager {
	CommandStack mUndoStack;
	CommandStack mRedoStack;
public:
	void Init();
	void ExecuteCommand(SharedPtr<Command> command);
	void Undo();
	void Redo();
};
#endif //COMMANDMANAGER_H