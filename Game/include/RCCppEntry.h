#pragma once

struct IRCCppEntry
{
    virtual void Update(float dt) = 0;
    virtual void Shutdown() = 0;
};