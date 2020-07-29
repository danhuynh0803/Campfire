#include "Audio/AudioSystem.h"
#include "Audio/AudioLayer.h"

AudioLayer::AudioLayer()
    : Layer("AudioLayer")
{
}

void AudioLayer::OnAttach()
{
    // Init audio
    AudioSystem::Init();
    AudioSystem::PlaySound("../Assets/Audio/xmas.wav");
}

void AudioLayer::OnDetach()
{
    AudioSystem::Shutdown();
}

void AudioLayer::OnUpdate(float dt)
{
    AudioSystem::OnUpdate(dt);
}

void AudioLayer::OnImGuiRender()
{

}

void AudioLayer::OnEvent(Event& event)
{

}


