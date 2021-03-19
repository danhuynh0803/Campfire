#pragma once
#ifndef RCCPPENTRY_H
#define RCCPPENTRY_H
#include "RCCppInterfaceID.h"
#include "RuntimeObjectSystem/IObject.h"


struct IRCCppEntry : public TInterface<IID_IRCCPP_ENTRY, IObject>
{
    virtual void Update(float dt) = 0;
    virtual void Shutdown() = 0;
};
#endif