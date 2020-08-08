#include "Core/Application.h"
#include "Core/FileSystem.h"
#include "Core/Time.h"
#include "Audio/AudioSystem.h"
#include "Renderer/Renderer.h"
#include "Editor/EditorLayer.h"

EditorLayer::EditorLayer()
    : Layer("Editor")
{
    activeScene = CreateSharedPtr<Scene>();
}

void EditorLayer::OnAttach()
{
    editorCamera = CreateSharedPtr<Camera>(1600, 900, 0.1f, 1000.0f);
    cameraController.SetActiveCamera(
            editorCamera,
            glm::vec3(0.0f, 3.0f, 10.0f),
            glm::vec3(0.0f, 20.0f, 0.0f)
    );
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(float dt)
{
    if (state == State::STOP)
    {
        cameraController.OnUpdate(dt);
    }

    activeScene->OnUpdate(dt);

    Renderer::BeginScene(*editorCamera);

    if (state == State::STOP)
    {
        activeScene->OnRenderEditor(dt, *editorCamera);
    }
    else
    {
        Time::timeScale = (state == State::PAUSE) ? 0.0f : 1.0f;
        activeScene->OnRenderRuntime(dt);
    }

    Renderer::EndScene();
}

void EditorLayer::OnImGuiRender()
{
    ImGui::Begin("Editor Camera Info");
    ImGui::DragFloat3("Position", (float*)&cameraController.position);
    bool prevState = editorCamera->isPerspective;
    ImGui::Checkbox("Is Perspective", &editorCamera->isPerspective);

    if (editorCamera->isPerspective)
        ImGui::DragFloat("Vertical FOV", &editorCamera->vFov);
    else
        ImGui::DragFloat("Size", &editorCamera->size);

    ImGui::DragFloat("Near Plane", &editorCamera->nearPlane);
    ImGui::DragFloat("Far Plane", &editorCamera->farPlane);
    ImGui::DragFloat("Depth", &editorCamera->depth);

    ImGui::Text("Viewport Rect");
    ImGui::DragFloat("x", &editorCamera->x);
    ImGui::DragFloat("y", &editorCamera->y);
    ImGui::DragFloat("w", &editorCamera->width);
    ImGui::DragFloat("h", &editorCamera->height);

    editorCamera->SetProjection();

    ImGui::End();

    // Menu bar
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

    // Various widgets
    if (showHierarchy)
    {
        wHierarchy.ShowHierarchy(activeScene, &showHierarchy);
    }
    if (wHierarchy.hasSelectedEntity && state != State::PLAY)
    {
        auto& entity = wHierarchy.GetSelectedEntity();

        wTransform.EditTransform(entity, *editorCamera);

        if (entity.HasComponent<CameraComponent>() && entity.HasComponent<TransformComponent>())
        {
            auto transform = entity.GetComponent<TransformComponent>().GetTransform();
            auto camera = entity.GetComponent<CameraComponent>().camera;

            camera->DrawFrustum(transform);
        }
    }
    if (showTransformSettings)
    {
        wTransform.ShowTransformSettings(&showTransformSettings);
    }

    // TODO convert to widgets
    if (showAudioSettings) { ShowAudioSettings(&showAudioSettings); }
    if (showConsole) { ShowConsole(&showConsole); }

    // Editor state buttons
    ImGui::Begin("Tool Bar");
    // TODO put various image buttons here
    // for now radio buttons to test
    static int currState = static_cast<int>(state);
    ImGui::RadioButton("Stop",  &currState, 0); ImGui::SameLine();
    ImGui::RadioButton("Play",  &currState, 1); ImGui::SameLine();
    ImGui::RadioButton("Pause", &currState, 2);
    state = static_cast<State>(currState);

    ImGui::End();

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
        ImGui::MenuItem("Move Tool", NULL, &showTransformSettings);

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
    if (state == State::STOP)
    {
        cameraController.OnEvent(event);
    }
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

