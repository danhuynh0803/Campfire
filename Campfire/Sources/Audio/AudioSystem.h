#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <glm/glm.hpp>

#include "Core/Base.h"
#include "Audio/FmodImpl.h"

class AudioSystem
{
public:
    static void Init();
    static void Shutdown();
    static void OnUpdate(float dt);

    // Studio related
    static void LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
    static void LoadEvent(const std::string& eventName);
    static void LoadSound(const std::string& soundName, bool is3d = true, bool isLooping = false, bool isStream = false);
    static void UnloadSound(const std::string& soundName);

    static void SetListenerPosition(const glm::vec3& pos = glm::vec3(0.0f), float volumedB = 0.0f);

    static void PlaySound(const std::string& filepath, const glm::vec3& pos = glm::vec3(0.0f), float volumedB = 0.0f);
    static void PlayEvent(const std::string& eventName, bool isImmediate = false);

    static void StopAllChannels();
    static void SetChannel3dPosition(int channelId, const glm::vec3& position);
    static void SetChannelVolume(int channelId, float volumedB);

    static bool IsPlaying(int channelId);
    static bool IsEventPlaying(const std::string& eventName);
    static float dBToVolume(float dB);
    static float VolumeTodB(float volume);

    static FMOD_VECTOR VectorToFmod(const glm::vec3& position);

private:
    static UniquePtr<FmodImpl> fmod;
};
#endif // AUDIO_SYSTEM_H
