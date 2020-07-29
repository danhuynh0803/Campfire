#include "Audio/AudioSystem.h"

#include "Core/Log.h"

FMOD::System* AudioSystem::coreSystem = nullptr;
FMOD::Studio::System* AudioSystem::studioSystem = nullptr;

void AudioSystem::Init()
{
    FMOD_RESULT result;
    result = FMOD::System_Create(&coreSystem);
    FMOD::System_Create(&coreSystem);
    if (result != FMOD_OK)
    {
        //LOG_ERROR("FMOD System could not be created: {0}", FMOD_ErrorString(result));
        exit(-1);
    }
    result = coreSystem->init(512, FMOD_INIT_NORMAL, 0);
    coreSystem->init(512, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK)
    {
        //LOG_ERROR("FMOD System could not be created: {0}", FMOD_ErrorString(result));
        exit(-1);
    }
}

void AudioSystem::Shutdown()
{
    // TODO
    // Release all created sounds and other cleanup needed
}

void AudioSystem::OnUpdate(float dt)
{
}

void AudioSystem::PlaySound(const char* filepath)
{
    FMOD::Channel *channel1 = 0, *channel2 = 0, *channel3 = 0;

    FMOD_VECTOR listenerPos = {0.0f, 0.0f, -1.0f};

    FMOD::Sound* bgm;
    coreSystem->createSound(filepath, FMOD_DEFAULT, nullptr, &bgm);
    //bgm->setMode(FMOD_LOOP_NORMAL);

    coreSystem->playSound(bgm, 0, false, &channel1);
}
