#include "Core/Application.h"
#include "Core/FileSystem.h"
#include "Core/Input.h"
#include "Core/Time.h"
#include "Core/Log.h"
#include "Audio/AudioSystem.h"
#include "Editor/EditorLayer.h"
#include "Physics/PhysicsManager.h"
#include "Scene/SceneLoader.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/SceneRenderer.h"

// TODO refactor task: FBOs should be handled by a renderer
SharedPtr<Framebuffer> gameCamFBO;
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

    gameCamFBO = Framebuffer::Create(1600, 900);

    postProcessShader = ShaderManager::Create("postprocess", "../Campfire/Shaders/postprocess.vert", "../Campfire/Shaders/postprocess.frag");
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(float dt)
{
    Time::timeScale = (state == State::PAUSE) ? 0.0f : 1.0f;

    if (Input::GetMod(MOD_KEY_CONTROL) && Input::GetKeyDown(KEY_R))
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

    if (state != State::STOP)
    {
        activeScene->OnUpdate(dt);
    }

    auto group = activeScene->GetAllEntitiesWith<CameraComponent, TransformComponent>();
    SharedPtr<Camera> mainGameCamera = nullptr;
    for (auto entity : group)
    {
        auto cameraComp = group.get<CameraComponent>(entity);
        if (cameraComp.isMain)
        {
            auto transform = group.get<TransformComponent>(entity);
            mainGameCamera = cameraComp.camera;

            mainGameCamera->pos = transform.position;
            mainGameCamera->RecalculateViewMatrix(transform.position, transform.rotation);
            mainGameCamera->SetProjection();
            break;
        }
    }

    if (mainGameCamera)
    {
        gameCamFBO->Bind();
        SceneRenderer::BeginScene(activeScene, *mainGameCamera);
        cameraController.OnUpdate(dt);
        activeScene->OnRender(dt, *mainGameCamera, state == State::PLAY);
        SceneRenderer::EndScene();
        gameCamFBO->Unbind();
    }

    SceneRenderer::BeginScene(activeScene, *editorCamera);
    cameraController.OnUpdate(dt);
    activeScene->OnRender(dt, *editorCamera, state == State::PLAY);
    SceneRenderer::EndScene();
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

        if (ImGui::BeginMenu("Campfire"))
        {
            ShowMenuCampfire();
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
    if (wHierarchy.hasSelectedEntity)
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
        if (state == State::STOP)
        {
            PhysicsManager::ClearLists();
            PhysicsManager::SubmitEntity(entity);
            PhysicsManager::DebugDraw();
        }
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

    // Game Camera viewport
    ImGui::Begin("Game Preview");
    ImGui::Image((ImTextureID)gameCamFBO->GetColorAttachmentID(), ImVec2(ImGui::GetWindowHeight()*(16.0f/9.0f), ImGui::GetWindowHeight()), ImVec2(0, 1), ImVec2(1, 0));
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

void EditorLayer::ShowMenuCampfire()
{
    if (ImGui::MenuItem("GitHub Repo"))
    {
        #ifdef  _WIN32
            ShellExecuteA(NULL, "open", "https://github.com/danhuynh0803/Campfire", NULL, NULL, SW_SHOWDEFAULT);
        #endif //  _WIN32
    }
    if (ImGui::MenuItem("Troubleshoot/Report Issues"))
    {
        #ifdef  _WIN32
            ShellExecuteA(NULL, "open", "https://github.com/danhuynh0803/Campfire/issues", NULL, NULL, SW_SHOWDEFAULT);
        #endif //  _WIN32
    }
    if (ImGui::MenuItem("Documentation/Wiki"))
    {
        #ifdef  _WIN32
            ShellExecuteA(NULL, "open", "https://github.com/danhuynh0803/Campfire/wiki", NULL, NULL, SW_SHOWDEFAULT);
        #endif //  _WIN32
    }
    if (ImGui::MenuItem("Contact us"))
    {
        #ifdef  _WIN32
            //ShellExecuteA(0, "open", ":name@domain.com?subject=Hello&Cc=xxx@domai%20n.com&bcc=xxx@domain.com.&body=Hello%20world", 0, 0, 1);
            ShellExecuteA(0, "open", "mailto:ray810815@gmail.com?subject=Campfire&body=Hello%20", 0, 0, 1);
        #endif //  _WIN32
    }
    ImGui::Separator();
    if (ImGui::MenuItem("About"))
    {

    }
}

void EditorLayer::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(EditorLayer::OnWindowResize));

    cameraController.OnEvent(event);
}

// TODO remove later.. just for testing FBO
bool EditorLayer::OnWindowResize(WindowResizeEvent& e)
{
    gameCamFBO->Resize(e.GetWidth(), e.GetHeight(), 0, true);
    //sceneCamFBO->Resize(e.GetWidth(), e.GetHeight(), 0, true);
    return false;
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

