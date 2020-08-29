#pragma once

#include <string>

class AudioSource
{
public:
    void Play();
    void Pause();
    void Unpause();
    void PlayDelayed(float delay);
    //void PlayOneShot(AudioClip, float volumeScale = 1.0f);
    void Stop();

public:
    std::string clipPath;
    bool mute = false;
    bool playOnAwake = false;
    bool loop = false;

    uint8_t priority = 128;
    float volume = 1.0f;
    float pitch = 1.0f;
    float stereoPan = 0.0f;
private:

};
