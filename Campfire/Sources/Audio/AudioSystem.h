#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include <string>
#include <map>
#include <vector>
#include <math.h>

#include "Core/Base.h"
#include "Audio/FmodImpl.h"

class AudioSystem
{
public:
    static void Init();
    static void Shutdown();
    static void OnUpdate(float dt);

    static void AddSound();
    static void PlaySound(const char* filepath);

private:
    static UniquePtr<FmodImpl> fmod;
};
#endif // AUDIO_SYSTEM_H
