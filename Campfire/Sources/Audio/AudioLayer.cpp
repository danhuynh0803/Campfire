#include "Audio/AudioSystem.h"
#include "Audio/AudioLayer.h"
#include "Core/FileSystem.h"

#include <imgui.h>

AudioLayer::AudioLayer()
    : Layer("AudioLayer")
{
}

void AudioLayer::OnAttach()
{
    // Init audio
    AudioSystem::Init();
    std::string BGM = "../Assets/Audio/test.wav";
    AudioSystem::PlaySound(BGM);
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


