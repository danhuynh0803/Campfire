#ifndef FMOD_IMPL_H
#define FMOD_IMPL_H

#include <fmod_studio.hpp>
#include <fmod.hpp>

struct FmodImpl
{
    FmodImpl()
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
