#pragma once
#ifndef TESTZ_H
#define TESTZ_H
#include "RuntimeObjectSystem/IObject.h"
#include "RCCppInterfaceID.h"

struct ITest : public TInterface<IID_IRCCPP_Test, IObject>
{
	virtual void OnUpdate(float dt) = 0;
};
#endif