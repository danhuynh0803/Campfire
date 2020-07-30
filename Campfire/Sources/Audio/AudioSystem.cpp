#include "Audio/AudioSystem.h"

#include "Core/Log.h"

UniquePtr<FmodImpl> AudioSystem::fmod;

void AudioSystem::Init()
{
    fmod = CreateUniquePtr<FmodImpl>();
}

void AudioSystem::Shutdown()
{
    // TODO
    // Release all created sounds and other cleanup needed
}

void AudioSystem::OnUpdate(float dt)
{
}

void AudioSystem::PlaySound(const std::string& filepath, const glm::vec3& pos, float volumedB)
{
    FMOD::Channel *channel1 = 0, *channel2 = 0, *channel3 = 0;

    FMOD_VECTOR listenerPos = {0.0f, 0.0f, -1.0f};

    FMOD::Sound* bgm;
    fmod->coreSystem->createSound(filepath.c_str(), FMOD_DEFAULT, nullptr, &bgm);
    //bgm->setMode(FMOD_LOOP_NORMAL);

    fmod->coreSystem->playSound(bgm, 0, false, &channel1);
}
