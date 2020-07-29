#ifndef AUDIO_LAYER_H
#define AUDIO_LAYER_H

#include "Core/Layer.h"
#include "Events/Event.h"

class AudioLayer : public Layer
{
public:
    AudioLayer();
    ~AudioLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& event) override;

    int masterVolume = 50;

private:
    std::vector<std::string> clips;
};


#endif // AUDIO_LAYER_H
