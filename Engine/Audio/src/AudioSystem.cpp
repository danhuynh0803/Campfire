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
}

void AudioSystem::OnUpdate(float dt)
{
    fmod->OnUpdate(dt);
}

void AudioSystem::LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
    auto foundIt = fmod->banks.find(bankName);
    if (foundIt != fmod->banks.end())
    {
        return; // bank already exists in map, no need to load
    }

    FMOD::Studio::Bank* bank;
    fmod->studioSystem->loadBankFile(bankName.c_str(), flags, &bank);
    if (bank)
    {
        fmod->banks[bankName] = bank;
    }
}

void AudioSystem::LoadEvent(const std::string& eventName)
{
    auto foundIt = fmod->events.find(eventName);
    if (foundIt != fmod->events.end())
    {
        return;
    }

    FMOD::Studio::EventDescription* eventDescription = nullptr;
    fmod->studioSystem->getEvent(eventName.c_str(), &eventDescription);
    if (eventDescription)
    {
        FMOD::Studio::EventInstance* eventInstance = nullptr;
        eventDescription->createInstance(&eventInstance);
        if (eventInstance)
        {
            fmod->events[eventName] = eventInstance;
        }
    }
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

void AudioSystem::SetListenerAttributes(int listener, const glm::vec3& pos, const glm::vec3& velocity)
{
    FMOD_3D_ATTRIBUTES attribs;
    attribs.position = VectorToFmod(pos);
    attribs.velocity = VectorToFmod(velocity);

    fmod->studioSystem->setListenerAttributes(listener, &attribs);
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
    auto foundIt = fmod->events.find(eventName);
    if (foundIt == fmod->events.end())
    {
        LoadEvent(eventName);
        foundIt = fmod->events.find(eventName);
        if (foundIt == fmod->events.end())
        {
            LOG_WARN("Event could not be loaded: {0}", eventName);
            return;
        }
    }
    // Play the event
    foundIt->second->start();
}

void AudioSystem::StopEvent(const std::string& eventName, bool isImmediate)
{
    auto foundIt = fmod->events.find(eventName);
    if (foundIt == fmod->events.end())
    {
        // Event doesn't exist
        return;
    }

    FMOD_STUDIO_STOP_MODE mode;
    mode = isImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
    foundIt->second->stop(mode);
}

void AudioSystem::StopAllChannels()
{
    for (auto& mapIt : fmod->channels)
    {
        mapIt.second->stop();
    }
}

void AudioSystem::SetChannel3dPosition(int channelId, const glm::vec3& position)
{
    auto foundIt = fmod->channels.find(channelId);

    if (foundIt == fmod->channels.end())
    {
        // channel specified is not currently used
        return;
    }
    FMOD_VECTOR pos = VectorToFmod(position);
    foundIt->second->set3DAttributes(&pos, nullptr);
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
    auto foundIt = fmod->channels.find(channelId);
    if (foundIt == fmod->channels.begin())
    {
        return false;
    }

    bool isPlaying;
    fmod->channels[channelId]->isPlaying(&isPlaying);
    return isPlaying;
}

bool AudioSystem::IsEventPlaying(const std::string& eventName)
{
    auto foundIt = fmod->events.find(eventName);
    if (foundIt == fmod->events.begin())
    {
        return false;
    }

    FMOD_STUDIO_PLAYBACK_STATE* state = nullptr;
    if (foundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING)
    {
        return true;
    }
    return false;
}

float AudioSystem::dBToVolume(float dB)
{
    // Volume = 10^(dB/20)
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
