#include <cmath>
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
    fmod->OnUpdate(dt);
}

void AudioSystem::LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
}

void AudioSystem::LoadEvent(const std::string& eventName)
{
}

void AudioSystem::LoadSound(const std::string& soundName, bool is3d, bool isLooping, bool isStream)
{
    auto it = fmod->sounds.find(soundName);
    if (it != fmod->sounds.end())
    {
        // Sound is already loaded into our sound map
        return;
    }

    // sound options
    FMOD_MODE mode = FMOD_DEFAULT;
    mode |= is3d ? FMOD_3D : FMOD_2D;
    mode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    mode |= isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    FMOD::Sound* sound = nullptr;
    fmod->coreSystem->createSound(soundName.c_str(), mode, nullptr, &sound);

    // Add to sound map if sound created successfully
    if (sound)
    {
        fmod->sounds[soundName] = sound;
    }
}

void AudioSystem::UnloadSound(const std::string& soundName)
{
    auto it = fmod->sounds.find(soundName);

    // Check if sound to unload is even in map
    if (it == fmod->sounds.end()) return;

    it->second->release();
    fmod->sounds.erase(it);
}

void AudioSystem::SetListenerPosition(const glm::vec3& pos, float volumedB)
{
}

int AudioSystem::PlaySound(const std::string& soundName, const glm::vec3& pos, float volumedB)
{
    int channelId = fmod->nextChannelId++;
    auto foundIt = fmod->sounds.find(soundName);
    if (foundIt == fmod->sounds.end())
    {
        // Sound doesnt yet exist in the map
        // so load it
        LoadSound(soundName);
        foundIt = fmod->sounds.find(soundName);
        if (foundIt == fmod->sounds.end())
        {
            return channelId;
        }
    }

    FMOD::Channel* channel = nullptr;
    fmod->coreSystem->playSound(foundIt->second, nullptr, true, &channel);
    if (channel)
    {
        FMOD_MODE currMode;
        foundIt->second->getMode(&currMode);
        if (currMode & FMOD_3D)
        {
            FMOD_VECTOR soundPos = VectorToFmod(pos);
            channel->set3DAttributes(&soundPos, nullptr);
        }
        channel->setVolume(dBToVolume(volumedB));
        channel->setPaused(false);
        fmod->channels[channelId] = channel;
    }

    return channelId;
}

void AudioSystem::PlayEvent(const std::string& eventName, bool isImmediate)
{
}

void AudioSystem::StopAllChannels()
{
}

void AudioSystem::SetChannel3dPosition(int channelId, const glm::vec3& position)
{
    auto foundIt = fmod->channels.find(channelId);

    if (foundIt == fmod->channels.end())
    {
        // channel specified is not currently used
        return;
    }
    foundIt->second->set3DAttributes(&VectorToFmod(position), nullptr);
}

void AudioSystem::SetChannelVolume(int channelId, float dB)
{
    auto foundIt = fmod->channels.find(channelId);

    if (foundIt == fmod->channels.end())
    {
        // channel specified is not currently used
        return;
    }

    foundIt->second->setVolume(dBToVolume(dB));
}

bool AudioSystem::IsPlaying(int channelId)
{
    return false;
}

bool AudioSystem::IsEventPlaying(const std::string& eventName)
{
    return false;
}

float AudioSystem::dBToVolume(float dB)
{
    return powf(10, 0.05f * dB);
}

float AudioSystem::VolumeTodB(float volume)
{
    return 20 * log10f(volume);
}

FMOD_VECTOR AudioSystem::VectorToFmod(const glm::vec3& position)
{
    FMOD_VECTOR vec;
    vec.x = position.x;
    vec.y = position.y;
    vec.z = position.z;

    return vec;
}
