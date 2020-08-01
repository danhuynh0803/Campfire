#include "Audio/FmodImpl.h"
#include <vector>

FmodImpl::FmodImpl()
{
    /*
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
    */

    FMOD_RESULT result = FMOD::Studio::System::create(&studioSystem);
    if (result != FMOD_OK)
    {
        exit(-1);
    }
    studioSystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
    // Grab the core system, which is created along with studio system
    studioSystem->getCoreSystem(&coreSystem);
}

FmodImpl::~FmodImpl()
{
    studioSystem->unloadAll();
    studioSystem->release();
}

void FmodImpl::OnUpdate(float dt)
{
    std::vector<ChannelMap::iterator> stoppedChannels;
    for (auto it = channels.begin(), itEnd = channels.end(); it != itEnd; ++it)
    {
        bool isPlaying = false;
        it->second->isPlaying(&isPlaying);
        if (!isPlaying)
        {
            stoppedChannels.push_back(it);
        }
    }

    for (auto& it : stoppedChannels)
    {
        channels.erase(it);
    }
    studioSystem->update();
}
