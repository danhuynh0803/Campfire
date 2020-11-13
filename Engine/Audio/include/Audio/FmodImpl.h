#ifndef FMOD_IMPL_H
#define FMOD_IMPL_H

#include <fmod_studio.hpp>
#include <fmod.hpp>

#include <map>
#include <string>

struct FmodImpl
{
    FmodImpl();
    ~FmodImpl();

    void OnUpdate(float dt);

    FMOD::System* coreSystem;
    FMOD::Studio::System* studioSystem;

    int nextChannelId;

    typedef std::map<std::string, FMOD::Sound*> SoundMap;
    typedef std::map<int, FMOD::Channel*> ChannelMap;
    typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
    typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;

    SoundMap sounds;
    ChannelMap channels;
    EventMap events;
    BankMap banks;
};

#endif // FMOD_IMPL_H
