#pragma once
#ifndef TESTY_H
#define TESTY_H
#include "TestZ.h"
#include "RuntimeObjectSystem/ObjectInterfacePerModule.h"
#include "RCCppInterfaceID.h"

struct Test : public ITest
{
	Test(){}
	virtual void Test::OnUpdate(float dt);
};
#endif