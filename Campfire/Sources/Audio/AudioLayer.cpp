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

    clips.reserve(5);
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
    ImGui::Begin("Audio System");

    ImGui::Text("Sound Settings");
    ImGui::SliderInt("Volume", &masterVolume, 0, 100);

    ImGui::Separator();

    ImGui::Text("Clips");
    std::string clipName;
    if (ImGui::Button("Add Audio Clip"))
    {
        //std::string path = FileSystem::OpenFile("*.wav");
        //if (!path.empty())
        {
            clips.push_back("Empty");
        }
    }

    ImGui::NewLine();

    for (size_t i = 0; i < clips.size(); ++i)
    {
        if (clips[i].empty()) { clipName = "Blank"; }
        else { clipName = clips[i]; }
        if (ImGui::Button(clipName.c_str()))
        {
            std::string path = FileSystem::OpenFile("*.wav");
            if (!path.empty())
            {
                clips[i] = path;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Play"))
        {
            AudioSystem::PlaySound(clips[i].c_str());
        }
    }

    ImGui::End();
}

void AudioLayer::OnEvent(Event& event)
{

}


