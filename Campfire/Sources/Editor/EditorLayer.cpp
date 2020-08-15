#include "Core/Application.h"
#include "Core/FileSystem.h"
#include "Core/Input.h"
#include "Core/Time.h"
#include "Core/Log.h"
#include "Audio/AudioSystem.h"
#include "Renderer/Renderer.h"
#include "Editor/EditorLayer.h"
#include "Physics/PhysicsManager.h"
#include "Scene/SceneLoader.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Renderer2D.h"

SharedPtr<Framebuffer> fbo;
SharedPtr<Shader> postProcessShader;

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
            glm::vec3(0.0f, 5.0f, 10.0f),
            glm::vec3(-20.0f, 0.0f, 0.0f)
    );

    fbo = Framebuffer::Create(1600, 900);

    postProcessShader = ShaderManager::Create("postprocess", "../Campfire/Shaders/postprocess.vert", "../Campfire/Shaders/postprocess.frag");
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(float dt)
{
    Time::timeScale = (state == State::PAUSE) ? 0.0f : 1.0f;

    if (Input::GetMod(MOD_CONTROL) && Input::GetKeyDown(KEY_R))
    {
        ShaderManager::ReloadShaders();
    }

    if (startScene)
    {
        // Submit all entities with rbs to Physics
        PhysicsManager::ClearLists();
        //auto rbEntities = activeScene->GetAllEntitiesWith<RigidbodyComponent>();
        for (auto entityPair : activeScene->GetEntityMap())
        {
            PhysicsManager::SubmitEntity(entityPair.second);
        }
    }

    else if (stopScene)
    {
    }

    Renderer::BeginScene(*editorCamera);
    if (state == State::STOP)
    {
        cameraController.OnUpdate(dt);

        fbo->Bind();
        RenderCommand::SetClearColor(glm::vec4(0.1f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        activeScene->OnRenderEditor(dt, *editorCamera);
        fbo->Unbind();
    }
    else
    {
        activeScene->OnUpdate(dt);
        activeScene->OnRenderRuntime(dt);
    }
    Renderer::EndScene();

    //RenderCommand::SetClearColor(glm::vec4(1.0f));
    //glClear(GL_COLOR_BUFFER_BIT);
    //Renderer2D::DrawPostProcessQuad(postProcessShader, fbo->GetColorAttachmentID());
}

void EditorLayer::OnImGuiRender()
{
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
    if (showLog)
    {
        Log::ShowLog(&showLog);
    }
    if (showHierarchy)
    {
        wHierarchy.ShowHierarchy(activeScene, &showHierarchy);
    }
    if (wHierarchy.hasSelectedEntity && state == State::STOP)
    {
        auto& entity = wHierarchy.GetSelectedEntity();

        wTransform.EditTransform(entity, *editorCamera);

        if (entity.HasComponent<CameraComponent>() && entity.HasComponent<TransformComponent>())
        {
            auto transform = entity.GetComponent<TransformComponent>().GetTransform();
            auto camera = entity.GetComponent<CameraComponent>().camera;

            camera->DrawFrustum(transform);
        }

        // FIXME: figure out a way to draw colliders without submitting to bullet each time
        PhysicsManager::ClearLists();
        PhysicsManager::SubmitEntity(entity);
        PhysicsManager::DebugDraw();
    }

    if (showTransformSettings)
    {
        wTransform.ShowTransformSettings(&showTransformSettings);
    }

    // TODO convert to widgets
    if (showAudioSettings) { ShowAudioSettings(&showAudioSettings); }
    if (showConsole) { ShowConsole(&showConsole); }
    if (showEditorCameraSettings) { ShowEditorCameraSettings(&showEditorCameraSettings); }

    // Editor state buttons
    ImGui::Begin("Tool Bar");
    // TODO put various image buttons here
    // for now radio buttons to test
    static int prevState = -1;
    static int currState = static_cast<int>(state);

    prevState = currState;
    ImGui::RadioButton("Stop",  &currState, 0); ImGui::SameLine();
    ImGui::RadioButton("Play",  &currState, 1); ImGui::SameLine();
    ImGui::RadioButton("Pause", &currState, 2);
    state = static_cast<State>(currState);

    startScene = (currState == 1 && prevState == 0) ? true : false;
    stopScene  = (currState == 0 && prevState != 0) ? true : false;

    ImGui::End();

    //ImGui::Begin("Scene");
    //auto viewportSize = ImGui::GetContentRegionAvail();
    //ImGui::End();


    // Viewport
    ImGui::Begin("Viewport");
    ImGui::Image((ImTextureID)fbo->GetColorAttachmentID(), ImVec2(ImGui::GetWindowHeight()*(16.0f/9.0f), ImGui::GetWindowHeight()), ImVec2(0, 1), ImVec2(1, 0));
    //ImGui::Image((ImTextureID)fbo->GetColorAttachmentID(), ImVec2(800, 450), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}

void EditorLayer::ClearScene()
{
    activeScene.reset();
    wHierarchy.Reset();
}

void EditorLayer::ShowMenuFile()
{
    if (ImGui::MenuItem("New Scene"))
    {
        ClearScene();
        activeScene = SceneLoader::LoadNewScene();
    }
    if (ImGui::MenuItem("Open Scene"))
    {
        std::string loadPath = FileSystem::OpenFile("Campfire Files(*.cf)\0");
        if (!loadPath.empty())
        {
            ClearScene();
            activeScene = SceneLoader::LoadScene(loadPath);
        }
    }
    if (ImGui::MenuItem("Save", "Ctrl+S"))
    {
        SceneLoader::SaveCurrentScene(activeScene);
    }
    if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S"))
    {
        std::string savePath = FileSystem::SaveFile("Campfire Files(*.cf)");
        if (!savePath.empty())
        {
            SceneLoader::SaveScene(activeScene, savePath);
        }
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
        ImGui::MenuItem("Log", NULL, &showLog);
        ImGui::MenuItem("Console", NULL, &showConsole);
        ImGui::MenuItem("Hierarchy", NULL, &showHierarchy);
        ImGui::MenuItem("Inspector", NULL, &showInspector);
        ImGui::MenuItem("Move Tool", NULL, &showTransformSettings);
        ImGui::MenuItem("Editor Camera", NULL, &showEditorCameraSettings);

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

void EditorLayer::ShowEditorCameraSettings(bool* isOpen)
{
    ImGui::Begin("Editor Camera Settings", isOpen);

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

