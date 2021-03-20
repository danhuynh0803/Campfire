#pragma once
#ifndef RCCPPSINGLETON_H
#define RCCPPSINGLETON_H

#include "RuntimeObjectSystem/IObject.h"
#include "IObjectUtils.h"

class MainObject : public IObject
{
public:

	MainObject()
	{
	}

	virtual void Serialize(ISimpleSerializer* pSerializer)
	{
	}

	virtual void Init(bool isFirstInit)
	{
		if (isFirstInit)
		{
			CreateObjects();
		}
	}

	~MainObject()
	{
		if (!IsRuntimeDelete())
		{
			// Delete all created objects
			delete IObjectUtils::GetUniqueObject("GameManager");

		}
	}

private:
	void CreateObjects()
	{
		// Construct unique objects (managers, util objects, etc)
		IObjectUtils::CreateUniqueObjectAndEntity("GameManager", "GameManager");

		//PerModuleInterface::g_pSystemTable->pLogSystem->Log(eLV_COMMENTS, "Created Objects\n");
	}
};
#endif // !RCCPPSINGLETON_H

//REGISTERSINGLETON(MainObject, true);