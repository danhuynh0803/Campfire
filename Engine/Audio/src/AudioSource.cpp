#include "Audio/AudioSource.h"
#include "Audio/AudioSystem.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Core/FileSystem.h"

void AudioSource::Play()
{
    AudioSystem::PlaySound(clipPath);
}

void AudioSource::Pause()
{
}

void AudioSource::Unpause()
{
}

void AudioSource::PlayDelayed(float delay)
{
}

void AudioSource::Stop()
{
}

void AudioSource::OnImGuiRender()
{
    std::string clipName = clipPath.empty() ? "Blank" : clipPath;
    if (ImGui::Button(clipName.c_str()))
    {
        std::string path = FileSystem::OpenFile("*.wav");
        if (!path.empty())
        {
            clipPath = path;
        }
    }

    ImGui::Checkbox("Mute", &mute);
    ImGui::Checkbox("Play on Awake", &playOnAwake);
    ImGui::Checkbox("Loop", &loop);

    ImGui::SliderInt("Priority", (int*)&priority, 0, 256);
    ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f);
    ImGui::SliderFloat("Pitch", &pitch, -3.0f, 3.0f);
    ImGui::SliderFloat("stereoPan", &stereoPan, -1.0f, 1.0f);
}
