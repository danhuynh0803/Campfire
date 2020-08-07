#include "Core/Application.h"
#include "Core/FileSystem.h"
#include "Audio/AudioSystem.h"
#include "Renderer/Renderer.h"
#include "Editor/EditorLayer.h"

SharedPtr<UniformBuffer> uboCamera;

Camera editorCamera(1600, 900, 0.1f, 100.0f);

EditorLayer::EditorLayer()
    : Layer("Editor")
{
    activeScene = CreateSharedPtr<Scene>();
}

void EditorLayer::OnAttach()
{
    uboCamera = UniformBuffer::Create();
    BufferLayout uboLayout =
    {
        { ShaderDataType::MAT4, "view"},
        { ShaderDataType::MAT4, "proj"},
        { ShaderDataType::MAT4, "viewProj"}
    };
    uboCamera->SetLayout(uboLayout, 0);
}

void EditorLayer::OnDetach()
{

}

void EditorLayer::OnUpdate(float dt)
{
    editorCamera.OnUpdate(dt);

    // TODO create a camera controller:
    // update active camera (either editor or game camera)
    // Set UBO data based on that active camera
    uboCamera->Bind();
    uboCamera->SetData((void*)glm::value_ptr(editorCamera.GetViewMatrix()), 0, sizeof(glm::mat4));
    uboCamera->SetData((void*)glm::value_ptr(editorCamera.GetProjMatrix()), sizeof(glm::mat4), sizeof(glm::mat4));
    uboCamera->SetData((void*)glm::value_ptr(editorCamera.GetViewProjMatrix()), 2*sizeof(glm::mat4), sizeof(glm::mat4));
    uboCamera->Unbind();

    activeScene->OnUpdate(dt);

    Renderer::BeginScene(editorCamera);

    activeScene->OnRenderEditor(dt, editorCamera);
    //activeScene->OnRenderRuntime(dt);

    Renderer::EndScene();
}

void EditorLayer::OnImGuiRender()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ShowMenuFile();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window"))
        {
            ShowMenuWindow();
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (showHierarchy)
    {
        wHierarchy.ShowHierarchy(activeScene, &showHierarchy);
    }
    if (wHierarchy.hasSelectedEntity)
    {
        wTransform.EditTransform(wHierarchy.GetSelectedEntity(), editorCamera);
    }

    // TODO convert to widgets
    if (showAudioSettings) { ShowAudioSettings(&showAudioSettings); }
    if (showConsole) { ShowConsole(&showConsole); }

    //ImGui::Begin("Scene");
    //auto viewportSize = ImGui::GetContentRegionAvail();
    //ImGui::End();
}

void EditorLayer::ShowMenuFile()
{
    if (ImGui::MenuItem("Save", "Ctrl+S"))
    {

    }
    if (ImGui::MenuItem("Save As.."))
    {

    }

    ImGui::Separator();

    if (ImGui::MenuItem("Quit"))
    {
        Application::Get().Close();
    }
}

void EditorLayer::ShowMenuWindow()
{
    if (ImGui::BeginMenu("General"))
    {
        ImGui::MenuItem("Console", NULL, &showConsole);
        ImGui::MenuItem("Hierarchy", NULL, &showHierarchy);
        ImGui::MenuItem("Inspector", NULL, &showInspector);

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Rendering"))
    {
        ImGui::MenuItem("Lighting Settings", NULL, &showLightSettings);
        ImGui::MenuItem("Light Explorer", NULL, &showLightExplorer);

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Audio"))
    {
        ImGui::MenuItem("Settings", NULL, &showAudioSettings);

        ImGui::EndMenu();
    }
}

void EditorLayer::OnEvent(Event& event)
{
    editorCamera.OnEvent(event);
}

// Editor Imgui Widgets
void EditorLayer::ShowAudioSettings(bool* isOpen)
{
    static int masterVolume = 50;
    static std::vector<std::string> clips;

    ImGui::Begin("Audio System", isOpen);

    ImGui::Text("Sound Settings");
    ImGui::SliderInt("Volume", &masterVolume, 0, 100);

    ImGui::Separator();

    ImGui::Text("Clips");
    std::string clipName;

    if (ImGui::Button("Stop All Channels"))
    {
        AudioSystem::StopAllChannels();
    }

    ImGui::Separator();

    if (ImGui::Button("Add Audio Clip"))
    {
        clips.push_back("Empty");
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

void EditorLayer::ShowConsole(bool* isOpen)
{
    // TODO make console widget
}

