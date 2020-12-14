#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <stack>
#include "Core/Base.h"
#include "Command.h"

typedef std::stack<UniquePtr<Command>> CommandStack;

class CommandManager {
	CommandStack mUndoStack;
	CommandStack mRedoStack;
public:
	void Init();
	void ExecuteCommand(UniquePtr<Command> command);
	void ExecuteCommand(const Command& command);
	void Undo();
	void Redo();
	void Clear();
};
#endif //COMMANDMANAGER_H