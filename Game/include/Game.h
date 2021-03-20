#pragma once
#ifndef GAME_H
#define GAME_H

typedef int AUEntityId;
#include "IEntitySystem.h"
#include "RuntimeCompiler/AUArray.h"
#include "RCCpp.h"
#include "IGame.h"

struct Game : public IGame
{
    bool Init();
    void Update(float dt);
    void Shutdown();
    class EntityUpdateProtector : public RuntimeProtector
    {
        public:
            AUDynArray<AUEntityId>  entities;
            float                   fDeltaTime;
            IEntitySystem* pEntitySystem;

        private:
            virtual void ProtectedFunc();
    };
    RCCpp* m_RCCpp;
    EntityUpdateProtector m_EntityUpdateProtector;
    ~Game();
};

#endif // ! GAME_H
