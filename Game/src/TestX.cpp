#include "TestX.h"
#include "RCCppSystemTable.h"
#include "RuntimeObjectSystem/IObject.h"
#include "RuntimeObjectSystem/ObjectInterfacePerModule.h"
#include "RuntimeObjectSystem/IObjectFactorySystem.h"

struct Test : public ITest, TInterface<2, IObject>
{
	Test()
	{
		//g_SystemTable->test = this;
		g_SystemTable->runtimeObjectSystem->GetObjectFactorySystem()->SetObjectConstructorHistorySize(10);
	}

	void OnUpdate(float dt) override
	{
		
	}
};

REGISTERCLASS(Test)