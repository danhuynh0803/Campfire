#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include <fmod_studio.hpp>
#include <fmod.hpp>
#include <string>
#include <map>
#include <vector>
#include <math.h>

#include "Core/Base.h"

class AudioSystem
{
public:
    static void Init();
    static void Shutdown();
    static void OnUpdate(float dt);

    static void AddSound();
    static void PlaySound(const char* filepath);

private:
    static FMOD::System* coreSystem;
    static FMOD::Studio::System* studioSystem;
};
#endif // AUDIO_SYSTEM_H
