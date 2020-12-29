#include "EditorLayer.h"

#include "Core/Application.h"
#include "Core/FileSystem.h"
#include "Core/Input.h"
#include "Core/Time.h"
#include "Core/Log.h"
#include "Audio/AudioSystem.h"
#include "Physics/PhysicsManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Skybox.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/SceneRenderer.h"
#include "Renderer/Text.h"
#include "Renderer/Renderer2D.h"
#include "ImGui/ImGuiLayer.h"
#include "Core/LogWidget.h"
#include "Util/Ray.h"
#include <Tracy.hpp>
#include "Renderer/Renderer.h"
#include "IconsFontAwesome5.h"
#include "Command/CommandManager.h"
#include "Scripting/LuaManager.h"

// TODO refactor task: FBOs should be handled by a renderer
SharedPtr<Framebuffer> gameCamFBO;
SharedPtr<Framebuffer> blurFBO;
SharedPtr<Framebuffer> editorCamFBO;
SharedPtr<Framebuffer> postprocessFBO;
std::vector<SharedPtr<Framebuffer>> pingpongFBOs;

// Curr display for the game viewport
static uint32_t currDisplay = 0;

EditorLayer::EditorLayer():
    Layer("Editor"),
    wAssetBrowser(this)
{
    LogWidget::Init();
    editorScene = CreateSharedPtr<Scene>();
    runtimeScene = CreateSharedPtr<Scene>(false);
    activeScene = editorScene;
}

void EditorLayer::OnAttach()
{
    ImGuiIO& io = ImGui::GetIO();
    //setup FontAwesome Icon Fonts
    io.Fonts->AddFontDefault();
    ImFontConfig icons_config;
    //must be static for some reason...
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    std::string FontAwesomeFontFilePath = FONTS + "/fa-solid-900.ttf";
    float iconFontSize = 16.0f;
    io.Fonts->AddFontFromFileTTF(FontAwesomeFontFilePath.c_str(), iconFontSize, &icons_config, icons_ranges);


    // TODO move all these resolution numbers into some global that can be accessed
    editorCamera = CreateSharedPtr<Camera>(1920, 1080, 0.1f, 1000.0f);
    cameraController.SetActiveCamera(
            editorCamera,
            glm::vec3(0.0f, 5.0f, 10.0f),
            glm::vec3(-20.0f, 0.0f, 0.0f)
    );

    // TODO set up via render pass and pipeline
    FramebufferSpec sceneSpec = {
        1920, 1080,
        GL_RGBA16F,
        2 // # of color attachments
    };
    gameCamFBO = Framebuffer::Create(sceneSpec);
    editorCamFBO = Framebuffer::Create(sceneSpec);
    blurFBO = Framebuffer::Create(sceneSpec);

    FramebufferSpec blurSpec = {
        1920, 1080,
        GL_RGBA16F,
    };
    for (int i = 0; i < 2; ++i)
    {
        pingpongFBOs.emplace_back(Framebuffer::Create(blurSpec));
    }

    FramebufferSpec viewportSpec = {
        1920, 1080, GL_RGBA
    };
    postprocessFBO = Framebuffer::Create(viewportSpec);

    // TODO move to renderer
    VAO = VertexArray::Create();
    VAO->Bind();
    VBO = VertexBuffer::Create(3 * 3 * 8 * sizeof(float));
    VBO->Bind();
    lineShader = ShaderManager::Get("line");
    BufferLayout layout =
    {
        { ShaderDataType::FLOAT3, "aPos" },
        { ShaderDataType::FLOAT3, "aColor" }
    };
    VBO->SetLayout(layout);
    VAO->AddVertexBuffer(VBO);

    uint32_t indices[] =
    {
        0, 1,   1, 2,   2, 3,   3, 0,
        4, 5,   5, 6,   6, 7,   7, 4,
        0, 4,   1, 5,   2, 6,   3, 7
    };

    SharedPtr<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
    VAO->SetIndexBuffer(indexBuffer);

    VBO->Unbind();
    VAO->Unbind();

    // Load up resources
    //{ // Load png
    //    auto files = FileSystem::GetAllFiles(ASSETS.c_str(), ".png");
    //    for (auto path : files)
    //    {
    //        ResourceManager::GetTexture2D(path.string());
    //    }
    //}

    //{ // Load jpg
    //    auto files = FileSystem::GetAllFiles(ASSETS.c_str(), ".jpg");
    //    for (auto path : files)
    //    {
    //        ResourceManager::GetTexture2D(path.string());
    //    }
    //}

    // Bloom
    blurShader = ShaderManager::Get("blur");

    postprocessShader = ShaderManager::Get("postprocess");
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::ProcessUserInput()
{
    ZoneScoped;

    //Undo Redo
    if (Input::GetMod(MOD_KEY_CONTROL) && Input::GetKeyDown(KEY_Z))
    {
        CommandManager::Undo();
    }

    if (Input::GetMod(MOD_KEY_CONTROL) && Input::GetKeyDown(KEY_Y))
    {
        CommandManager::Redo();
    }

    if (Input::GetMod(MOD_KEY_CONTROL) && Input::GetKeyDown(KEY_R))
    {
        ShaderManager::ReloadShaders();
    }

    if (Input::GetKeyDown(KEY_B))
    {
        showBoundingBoxes ^= 1;
    }

    // Focus on selected object
    float focusDistance = 15.0f; // have this be dynamic
    if (Input::GetKeyDown(KEY_F))
    {
        auto entity = wHierarchy.GetSelectedEntity();
        if (entity)
        {
            glm::mat4 view = editorCamera->GetViewMatrix();
            glm::vec3 camDir = -glm::vec3(view[0][2], view[1][2], view[2][2]);
            glm::vec3 entPos = entity.GetComponent<TransformComponent>().position;
            cameraController.position = entPos - camDir * focusDistance;
        }
    }

    // New Scene
    if (Input::GetMod(MOD_KEY_CONTROL) && Input::GetKeyDown(KEY_N))
    {
        //if (OpenConfirmPrompt("All unsaved progress will be lost!"))
        {
            ClearScene();
            editorScene = SceneManager::LoadNewScene();
            activeScene = editorScene;
        }
    }

    // Open scene
    if (Input::GetMod(MOD_KEY_CONTROL) && Input::GetKeyDown(KEY_O))
    {
        std::string loadPath = FileSystem::OpenFile("Campfire Files(*.cf)\0");
        if (!loadPath.empty())
        {
            //if (OpenConfirmPrompt("All unsaved progress will be lost!"))
            {
                ClearScene();
                editorScene = SceneManager::LoadScene(loadPath);
                activeScene = editorScene;
            }
        }
    }

    // Save current scene
    if (Input::GetMod(MOD_KEY_CONTROL, true)
        && Input::GetKeyDown(KEY_S))
    {
        SceneManager::SaveCurrentScene(activeScene);
    }

    // Save to a new scene file
    if (Input::GetMod(MOD_KEY_CONTROL | MOD_KEY_SHIFT)
        && Input::GetKeyDown(KEY_S))
    {
        std::string savePath = FileSystem::SaveFile("Campfire Files(*.cf)");
        if (!savePath.empty())
        {
            SceneManager::SaveScene(activeScene, savePath);
        }
    }

}

void EditorLayer::OnUpdate(float dt)
{
    ProcessUserInput();

    ZoneScoped;

    {
        auto group = activeScene->GetAllEntitiesWith<ActiveComponent>();
        for (auto handle : group)
        {
            Entity entity(handle, activeScene.get());
            bool isActive = entity.GetComponent<ActiveComponent>();
            //if (entity.GetComponent<ActiveComponent>())
            //    activeScene->EnableEntity(entity);
            //else
            //    activeScene->DisableEntity(entity);
        }
    }

    // Move inactive/active objects from registries

    float deltaTime = (state == State::PAUSE) ? 0.0f : dt;

    static int editorSelectedIndex = -1;
    if (startScene)
    {
        editorSelectedIndex = wHierarchy.GetSelectedIndex();
        // Enable playmode tint
        ImGuiLayer::SetPlayTheme();
        // FIXME: maybe better way of doing this?
        // Reset the runtimeScene entirely to avoid leftover data from instantiation events
        runtimeScene.reset();
        runtimeScene = CreateSharedPtr<Scene>(false);
        runtimeScene->CopyFrom(editorScene);
        activeScene = runtimeScene;
        activeScene->OnStart();
    }
    else if (stopScene)
    {
        // Disable playmode tint
        ImGuiLayer::SetStopTheme();
        wHierarchy.OverrideSelectedIndex(editorSelectedIndex);
        if (editorSelectedIndex == -1)
        {
            wHierarchy.Reset();
        }
        activeScene->OnStop();
        activeScene = editorScene;

        PhysicsManager::ClearLists();
        auto entityMap = activeScene->GetEntityMap();
        for (auto entityPair : entityMap)
        {
            PhysicsManager::SubmitEntity(entityPair.second);
        }
    }

    if (state != State::STOP)
    {
        activeScene->OnUpdate(deltaTime);
    }

    if (is2d)
    {
        // FIXME Issues with clicking when reorienting view
        //cameraController.LockViewTo2d();
        editorCamera->isPerspective = false;
    }
    else
    {
        editorCamera->isPerspective = true;
    }

    auto group = activeScene->GetAllEntitiesWith<CameraComponent, TransformComponent, RelationshipComponent>();
    SharedPtr<Camera> mainGameCamera = nullptr;
    for (auto entity : group)
    {
        auto comp = group.get<CameraComponent>(entity);

        if (!comp.isActive) { continue; }

        SharedPtr<Camera> selectedCamera = comp.camera;
        if (selectedCamera->targetDisplay == currDisplay)
        {
            mainGameCamera = selectedCamera;
            auto& transformComponent = group.get<TransformComponent>(entity);

            auto relationshipComponent = group.get<RelationshipComponent>(entity);
            // If entity is a child, then apply parents transform to it
            if (relationshipComponent.parent != entt::null)
            {
                Entity parent(relationshipComponent.parent, activeScene.get());
                auto parentTransform = parent.GetComponent<TransformComponent>();

                glm::mat4 transform = glm::mat4(1.0f);
                transform = glm::mat4(1.0f);
                glm::vec3 position    = transformComponent.position + parentTransform.position;
                glm::vec3 eulerAngles = transformComponent.euler;
                glm::vec3 scale = transformComponent.scale * parentTransform.scale;

                glm::vec3 parentEulerAngles = parentTransform.euler;
                glm::quat parentRotation = glm::quat(
                        glm::vec3(
                            glm::radians(parentEulerAngles.x),
                            glm::radians(parentEulerAngles.y),
                            glm::radians(parentEulerAngles.z)
                        )
                    );
                glm::vec3 rotationPosition = parentTransform.position + (parentRotation * (position - parentTransform.position));

                mainGameCamera->pos = rotationPosition;
                mainGameCamera->RecalculateViewMatrix(rotationPosition, transformComponent.euler + parentEulerAngles);
                mainGameCamera->SetProjection();
            }
            else
            {
                mainGameCamera->pos = transformComponent.position;
                mainGameCamera->RecalculateViewMatrix(transformComponent.position, transformComponent.euler);
                mainGameCamera->SetProjection();
            }
            break;
        }
    }

    if (mainGameCamera)
    {
        gameCamFBO->Bind();
            unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
            glDrawBuffers(2, attachments);

            RenderCommand::SetDrawMode(DrawMode::SHADED);

            switch (mainGameCamera->clearFlag)
            {
                default:
                case ClearFlag::SKYBOX:
                    SceneRenderer::BeginScene(activeScene, *mainGameCamera);
                    activeScene->skybox->DrawSkybox();
                    break;
                case ClearFlag::COLOR:
                    SceneRenderer::BeginScene(activeScene, *mainGameCamera);
                    RenderCommand::SetClearColor(mainGameCamera->backgroundColor);
                    RenderCommand::Clear();
                    break;
                case ClearFlag::DEPTH:
                    break;
                case ClearFlag::NONE:
                    break;
            }

            activeScene->OnRender(deltaTime, *mainGameCamera);
            SceneRenderer::EndScene();
        gameCamFBO->Unbind();
    }
    else
    {
        gameCamFBO->Bind();
            RenderCommand::SetClearColor(glm::vec4(0.5f, 0.0f, 0.5f, 1.0f));
            RenderCommand::Clear();
        gameCamFBO->Unbind();
    }

    blurFBO->Bind();
    {
        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);
        SceneRenderer::BeginScene(activeScene, *editorCamera);
        RenderCommand::SetDrawMode(drawMode);
        activeScene->OnRender(deltaTime, *editorCamera);
    }
    blurFBO->Unbind();

    editorCamFBO->Bind();
    {
        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);

        // Set editor cam to perspective just to draw the skybox
        bool isPerspective = editorCamera->isPerspective;
        editorCamera->isPerspective = true;
        SceneRenderer::BeginScene(activeScene, *editorCamera);
        editorCamera->isPerspective = isPerspective;

        if (drawSkybox) {
            RenderCommand::SetDrawMode(DrawMode::SHADED);
            activeScene->skybox->DrawSkybox();
        }
        else {
            RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
            RenderCommand::Clear();
        }

        RenderCommand::SetDrawMode(drawMode);

        SceneRenderer::BeginSceneWithoutClear(activeScene, *editorCamera);

        if (allowViewportCameraEvents)
        {
            if (Input::GetMouseButton(MOUSE_BUTTON_RIGHT))
                hasViewportAction = true;
        }

        if (hasViewportAction)
        {
            if (Input::GetMouseButton(MOUSE_BUTTON_RIGHT))
            {
                Application::Get().GetWindow().DisableCursor();
            }
            else
            {
                Application::Get().GetWindow().EnableCursor();
                hasViewportAction = false;
            }
        }

        if (allowViewportCameraEvents
            || hasViewportAction
        ) {
            cameraController.OnUpdate(dt);
        }

        activeScene->OnRender(deltaTime, *editorCamera);

        // TODO Change so that debug draw only shows BBs of selected objects
        if (wHierarchy.GetSelectedEntity())
        {
            PhysicsManager::DebugDraw();
        }

        if (showBoundingBoxes)
        {
            // TODO move this elsewhere later
            glm::vec3 color(1.0f, 1.0f, 1.0f);

            auto spriteGroup = activeScene->GetAllEntitiesWith<SpriteComponent>();

            for (auto e : spriteGroup)
            {
                Entity entity { e, activeScene.get() };
                // Need to apply transform to get the correct position of the BB
                SpriteComponent comp = entity.GetComponent<SpriteComponent>();

                glm::vec4 min = glm::vec4(comp.boundingBox.mMin, 1.0f);
                glm::vec4 max = glm::vec4(comp.boundingBox.mMax, 1.0f);

                GLfloat vertices[] =
                {
                    min.x, max.y, min.z, color.r, color.g, color.b,
                    min.x, min.y, min.z, color.r, color.g, color.b,
                    max.x, min.y, min.z, color.r, color.g, color.b,
                    max.x, max.y, min.z, color.r, color.g, color.b,

                    min.x, max.y, max.z, color.r, color.g, color.b,
                    min.x, min.y, max.z, color.r, color.g, color.b,
                    max.x, min.y, max.z, color.r, color.g, color.b,
                    max.x, max.y, max.z, color.r, color.g, color.b
                };

                VBO->SetData(vertices, sizeof(vertices));

                Renderer::DrawLines(lineShader, VAO, entity.GetComponent<TransformComponent>());
            }

            auto meshGroup = activeScene->GetAllEntitiesWith<MeshComponent>();
            for (auto e : meshGroup)
            {
                Entity entity { e, activeScene.get() };
                auto mesh = entity.GetComponent<MeshComponent>().mesh;
                // Skip if no mesh exists
                if (!mesh) { continue; }

                auto& submeshes = mesh->GetSubmeshes();
                for (auto submesh : submeshes)
                {
                    // Need to apply transform to get the correct position of the BB
                    glm::vec4 min = glm::vec4(submesh.boundingBox.mMin, 1.0f);
                    glm::vec4 max = glm::vec4(submesh.boundingBox.mMax, 1.0f);

                    GLfloat vertices[] =
                    {
                        min.x, max.y, min.z, color.r, color.g, color.b,
                        min.x, min.y, min.z, color.r, color.g, color.b,
                        max.x, min.y, min.z, color.r, color.g, color.b,
                        max.x, max.y, min.z, color.r, color.g, color.b,

                        min.x, max.y, max.z, color.r, color.g, color.b,
                        min.x, min.y, max.z, color.r, color.g, color.b,
                        max.x, min.y, max.z, color.r, color.g, color.b,
                        max.x, max.y, max.z, color.r, color.g, color.b
                    };

                    VBO->SetData(vertices, sizeof(vertices));

                    Renderer::DrawLines(lineShader, VAO, entity.GetComponent<TransformComponent>());
                }
            }
        }

        // Draw camera frustum
        auto& entity = wHierarchy.GetSelectedEntity();
        if (entity && entity.HasComponent<CameraComponent>() && entity.HasComponent<TransformComponent>())
        {
            auto camera = entity.GetComponent<CameraComponent>().camera;
            camera->DrawFrustum(entity.GetComponent<TransformComponent>());
        }

        // TODO keep for figuring out ortho raycasting
        //if (Input::GetMouseButton(MOUSE_BUTTON_LEFT))
        //{
        //    auto [mouseX , mouseY] = GetMouseViewportSpace();
        //    LOG_INFO("MouseX = {0}", mouseX);
        //    LOG_INFO("MouseY = {0}", mouseY);
        //    LOG_INFO("cameraW = {0}", editorCamera->width);
        //    LOG_INFO("cameraH = {0}", editorCamera->height);
        //    LOG_INFO("viewportW = {0}", maxViewportBound.x - minViewportBound.x);
        //    LOG_INFO("viewportH = {0}", maxViewportBound.y - minViewportBound.y);

        //    glm::vec3 rayOrig, rayDir;
        //    ScreenToWorldRay(
        //        mouseX,
        //        mouseY,
        //        editorCamera->width,
        //        editorCamera->height,
        //        editorCamera->GetViewMatrix(),
        //        editorCamera->GetProjMatrix(),
        //        rayOrig,
        //        rayDir
        //    );
        //    Ray ray(rayOrig, rayDir);

        //    //LOG_TRACE("rayOrig = ({0}, {1} {2})", rayOrig.x, rayOrig.y, rayOrig.z);
        //    //LOG_TRACE("rayDir = ({0}, {1} {2})", rayDir.x, rayDir.y, rayDir.z);

        //    glm::vec3 color(1.0f, 0.0f, 0.0f);
        //    GLfloat vertices[] =
        //    {
        //        rayOrig.x, rayOrig.y, rayOrig.z, color.r, color.g, color.b,
        //        rayDir.x, rayDir.y, rayDir.z, color.r, color.g, color.b,
        //        rayOrig.x, rayOrig.y, rayOrig.z, color.r, color.g, color.b,
        //        rayDir.x, rayDir.y, rayDir.z, color.r, color.g, color.b,

        //        rayOrig.x, rayOrig.y, rayOrig.z, color.r, color.g, color.b,
        //        rayDir.x, rayDir.y, rayDir.z, color.r, color.g, color.b,
        //        rayOrig.x, rayOrig.y, rayOrig.z, color.r, color.g, color.b,
        //        rayDir.x, rayDir.y, rayDir.z, color.r, color.g, color.b,
        //    };

        //    VBO->SetData(vertices, sizeof(vertices));

        //    Renderer::DrawLines(lineShader, VAO, glm::mat4(1.0f));
        //}

        SceneRenderer::EndScene();
    }
    editorCamFBO->Unbind();

    RenderCommand::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    RenderCommand::Clear();

    // Blur the image for blooming
    bool isHorizontal = true, isFirst = true;
    int maxIter = 10;
    blurShader->Bind();
    for (int i = 0; i < maxIter; ++i)
    {
        pingpongFBOs[isHorizontal]->Bind();
        blurShader->SetBool("isHorizontal", isHorizontal);
        glBindTexture(GL_TEXTURE_2D, isFirst
            //? editorCamFBO->GetColorAttachmentID(1)
            ? blurFBO->GetColorAttachmentID(1)
            : pingpongFBOs[!isHorizontal]->GetColorAttachmentID()
        );

        Renderer2D::DrawPostProcessQuad(blurShader, isFirst
            ? blurFBO->GetColorAttachmentID(1)
            //? editorCamFBO->GetColorAttachmentID(1)
            : pingpongFBOs[!isHorizontal]->GetColorAttachmentID()
        );

        isHorizontal = !isHorizontal;
        if (isFirst) { isFirst = false; }
    }
    pingpongFBOs[0]->Unbind();

    RenderCommand::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    RenderCommand::Clear();

    // Postprocess
    postprocessFBO->Bind();
    {
        RenderCommand::SetDrawMode(DrawMode::SHADED);
        postprocessShader->Bind();
        postprocessShader->SetInt("sceneTex", 0);
        postprocessShader->SetInt("bloomBlurTex", 1);
        postprocessShader->SetFloat("exposure", exposure);
        glBindTextureUnit(1, pingpongFBOs[0]->GetColorAttachmentID());
        Renderer2D::DrawPostProcessQuad(postprocessShader, editorCamFBO->GetColorAttachmentID());
    }
    postprocessFBO->Unbind();

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void EditorLayer::OnImGuiRender()
{
    static ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_None;
    // Enable Dockspace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockSpaceID = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockSpaceID, ImVec2(0.0f, 0.0f), dockFlags);
    }

    // Show AssetBrowser
    wAssetBrowser.OnImGuiRender(&showAssetBrowser);

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

    // Various widget windows
    LogWidget::Draw(&showLog);

    if (showHierarchy)
    {
        wHierarchy.ShowHierarchy(activeScene, editorCamera, &showHierarchy);
    }
    if (showTransformSettings)
    {
        wTransform.ShowTransformSettings(&showTransformSettings);
    }
    if (showInspector)
    {
        wInspector.ShowInspector(wHierarchy.GetSelectedEntity(), &showInspector);
    }

    // Editor viewport
    ImGui::Begin("Viewport");
    {
        // -------------------------------------------------
        // Scene Toolbar
        // Draw modes
        const char* drawModes[] =
        {
            "Shaded",
            "Wireframe",
        };
        int currMode = static_cast<int>(drawMode);
        ImGui::Combo("Draw Mode", &currMode, drawModes, IM_ARRAYSIZE(drawModes));
        drawMode = static_cast<DrawMode>(currMode);

        ImGui::SameLine();

        ImGui::Checkbox("2D", &is2d);

        ImGui::SameLine();

        const char* effects[] = {
            "Skybox",
            "Post-Process",
            "Particle Systems",
        };
        static bool* toggles[] = {
            &drawSkybox,
            &drawPostProcess,
            &drawParticleSystems,
        };

        if (ImGui::Button("Effects"))
        {
            ImGui::OpenPopup("EffectsTogglePopup");
        }
        if (ImGui::BeginPopup("EffectsTogglePopup"))
        {
            for (int i = 0; i < IM_ARRAYSIZE(effects); i++)
                ImGui::MenuItem(effects[i], "", toggles[i]);
            ImGui::EndPopup();
        }
        // -------------------------------------------------

        auto viewportOffset = ImGui::GetCursorPos();
        auto viewportSize = ImGui::GetContentRegionAvail();
        editorCamera->SetProjection((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

        // Loading bar if scene is saving or loading
        // TODO move to SceneManager
        //const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
        //const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);
        //ImGui::BufferingBar("Saving Scene", 0.5f, ImVec2(viewportSize.x, 15), bg, col);

        if (drawPostProcess) {
            ImGui::Image((ImTextureID)postprocessFBO->GetColorAttachmentID(), viewportSize, { 0, 1 }, { 1, 0 });
        }
        else {
            ImGui::Image((ImTextureID)editorCamFBO->GetColorAttachmentID(), viewportSize, { 0, 1 }, { 1, 0 });
        }

        auto windowSize = ImGui::GetWindowSize();
        ImVec2 minBound = ImGui::GetWindowPos();
        minBound.x += viewportOffset.x;
        minBound.y += viewportOffset.y;
        minViewportBound = { minBound.x, minBound.y };

        ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
        maxViewportBound = { maxBound.x, maxBound.y };
        allowViewportCameraEvents = ImGui::IsMouseHoveringRect(minBound, maxBound);

        // FIXME: gizmo not moving with runtimescene transform
        // disable in runtime, since moving in runtime causes crashes
        //if (state == State::STOP && wHierarchy.hasSelectedEntity)

        auto& entity = wHierarchy.GetSelectedEntity();
        if (entity)
        {
            float rw = (float)ImGui::GetWindowWidth();
            float rh = (float)ImGui::GetWindowHeight();
            ImGuizmo::SetOrthographic(!editorCamera->isPerspective);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, rw, rh);

            ImGuiIO io = ImGui::GetIO();
            if (allowViewportCameraEvents && !Input::GetMouseButtonDown(MOUSE_BUTTON_RIGHT))
            {
                if (Input::GetKeyDown(KEY_W))
                    wTransform.operation = ImGuizmo::TRANSLATE;
                if (Input::GetKeyDown(KEY_E))
                    wTransform.operation = ImGuizmo::ROTATE;
                if (Input::GetKeyDown(KEY_R))
                    wTransform.operation = ImGuizmo::SCALE;
            }

            wTransform.EditTransform(entity, *editorCamera);

            /*
            ImGuizmo::ViewManipulate(
                (float*)glm::value_ptr(editorCamera->GetViewMatrix()),
                100, // TODO
                ImVec2(io.DisplaySize.x - 128, 0),
                ImVec2(128, 128),
                0x10101010
            );
            */

            if (state == State::STOP)
            {
                PhysicsManager::ClearLists();
                PhysicsManager::SubmitEntity(entity);
                //PhysicsManager::DebugDraw();
            }
        }
    }
    ImGui::End();

    // TODO convert to widgets
    if (showAudioSettings) { ShowAudioSettings(&showAudioSettings); }
    if (showConsole) { ShowConsole(&showConsole); }
    if (showEditorCameraSettings) { ShowEditorCameraSettings(&showEditorCameraSettings); }

    // Editor state buttons
    ImGui::Begin("Tool Bar");
    {
        // TODO put various image buttons here
        // for now radio buttons to test
        static int prevState = -1;
        static int currState = static_cast<int>(state);

        // TODO replace with ImageButtons
        prevState = currState;
        // If State is stop, then show the play button
        // Else if State is play, then show the stop button
        ImGui::RadioButton("Stop", &currState, 0); ImGui::SameLine();
        ImGui::RadioButton("Play", &currState, 1); ImGui::SameLine();
        // If game is running, then enable step button
        ImGui::RadioButton("Pause", &currState, 2);
        state = static_cast<State>(currState);

        startScene = (currState != 0 && prevState == 0) ? true : false;
        stopScene = (currState == 0 && prevState != 0) ? true : false;
    }
    ImGui::End();

    //ImGui::Begin("PostProcess");
    //{
    //    ImGui::SliderFloat("Exposure", &exposure, 0.0f, 5.0f);
    //    auto viewportSize = ImGui::GetContentRegionAvail();
    //    //ImGui::Image((ImTextureID)pingpongFBOs[0]->GetColorAttachmentID(), viewportSize, { 0, 1 }, { 1, 0 });
    //    ImGui::Image((ImTextureID)postprocessFBO->GetColorAttachmentID(), viewportSize, { 0, 1 }, { 1, 0 });
    //}
    //ImGui::End();

    // Game Camera viewport
    ImGui::Begin("Game View");
    {
        ImGui::PushID(0);
        const char* displays[] = { "Display 1", "Display 2", "Display 3", "Display 4", "Display 5" };
        ImGui::Combo("", (int*)&currDisplay, displays, IM_ARRAYSIZE(displays));
        ImGui::PopID();

        ImGui::SameLine();

        ImGui::PushID(1);
        static int currResolutionIndex = 1; // TODO Save in a editor config file
        const char* resolutions[] = { "Free aspect", "16x9" };
        ImGui::Combo("", (int*)&currResolutionIndex, resolutions, IM_ARRAYSIZE(resolutions));
        ImGui::PopID();

        //ImGui::SameLine();

        ImGui::Checkbox("Maximize", &maximizeGameViewport);

        auto viewportSize = ImGui::GetContentRegionAvail();
        switch (currResolutionIndex)
        {
            case 0: // Free aspect
            {
                ImGui::Image((ImTextureID)gameCamFBO->GetColorAttachmentID(), viewportSize, { 0, 1 }, { 1, 0 });
                break;
            }
            case 1: // 16x9 resolution
            {
                ImVec2 fixed16x9Viewport = (viewportSize.x / viewportSize.y) > (16.0f / 9.0f)
                    ? ImVec2(viewportSize.y * (16.0f / 9.0f), viewportSize.y)
                    : ImVec2(viewportSize.x, viewportSize.x * (9.0f / 16.0f));
                glm::vec2 titleBarHeight(0, ImGui::GetCurrentWindow()->TitleBarHeight());
                glm::vec2 center =
                    0.5f *
                    (
                        glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y)
                        - glm::vec2(fixed16x9Viewport.x, fixed16x9Viewport.y)
                    )
                    + titleBarHeight
                ;

                ImGui::SetCursorPos({ center.x, center.y });
                ImGui::Image((ImTextureID)gameCamFBO->GetColorAttachmentID(), fixed16x9Viewport, { 0, 1 }, { 1, 0 });
                break;
            }
        }

    }
    ImGui::End();

    if (maximizeGameViewport
        && state == State::PLAY)
    {
        bool isOpen = true;
        // Scale down a bit just to be able to see the whole window
        ImGui::SetNextWindowSize(ImVec2(maxResolution.x * 0.95f, maxResolution.y * 0.95f));
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoDocking
            //| ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoSavedSettings
        ;
        ImGui::Begin("Game", &isOpen, flags);

        auto viewportSize = ImGui::GetContentRegionAvail();
        ImGui::Image((ImTextureID)gameCamFBO->GetColorAttachmentID(), viewportSize, { 0, 1 }, { 1, 0 });

        ImGui::End();
    }

    // Engine closing
    if (shouldOpenExitPrompt)
    {
        OpenClosePrompt();
    }
}

void EditorLayer::ClearScene()
{
    activeScene.reset();
    wHierarchy.Reset();
}

void EditorLayer::ShowMenuFile()
{
    ImGui::Text(ICON_FA_FILE);
    ImGui::SameLine();
    if (ImGui::MenuItem("New Scene", "Ctrl+N"))
    {
        //if (OpenConfirmPrompt("All unsaved progress will be lost!"))
        {
            ClearScene();
            editorScene = SceneManager::LoadNewScene();
            activeScene = editorScene;
        }
    }
    ImGui::Text(ICON_FA_FOLDER_OPEN);
    ImGui::SameLine();
    if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
    {
        std::string loadPath = FileSystem::OpenFile("Campfire Files(*.cf)\0");
        if (!loadPath.empty())
        {
            //if (OpenConfirmPrompt("All unsaved progress will be lost!"))
            {
                ClearScene();
                editorScene = SceneManager::LoadScene(loadPath);
                activeScene = editorScene;
            }
        }
    }
    ImGui::Text(ICON_FA_SAVE);
    ImGui::SameLine();
    if (ImGui::MenuItem("Save", "Ctrl+S"))
    {
        SceneManager::SaveCurrentScene(activeScene);
    }
    ImGui::Text(ICON_FA_SAVE);
    ImGui::SameLine();
    if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S"))
    {
        std::string savePath = FileSystem::SaveFile("Campfire Files(*.cf)");
        if (!savePath.empty())
        {
            SceneManager::SaveScene(activeScene, savePath);
        }
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
        FileSystem::OpenCampFireGithubRepo();
    }
    if (ImGui::MenuItem("Troubleshoot/Report Issues"))
    {
        FileSystem::OpenCampFireGithubRepoTroubleshoot();
    }
    if (ImGui::MenuItem("Documentation/Wiki"))
    {
        FileSystem::OpenCampFireGithubRepoWiki();
    }
    if (ImGui::MenuItem("Contact us"))
    {
        FileSystem::OpenEmail("mailto:ray810815@gmail.com?subject=Campfire&body=Hello%20");
    }
    ImGui::Separator();
    if (ImGui::MenuItem("About"))
    {

    }
}

void EditorLayer::OnEvent(Event& event)
{
    if (allowViewportCameraEvents
        || hasViewportAction
    ) {
        cameraController.OnEvent(event);
    }

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(EditorLayer::OnWindowResize));
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseClick));
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(EditorLayer::OnWindowClose));
}

//void EditorLayer::OnLuaEvent(LuaEvent& event)
//{
//    LuaEventDispatcher dispatcher(event);
//    dispatcher.Dispatch<LuaSetGlobalEvent>(BIND_EVENT_FN(EditorLayer::OnLuaSetGlobal));
//}

void EditorLayer::ScreenToWorldRay(
    float mouseX, float mouseY,
    int screenWidth, int screenHeight,
    glm::mat4 viewMatrix,
    glm::mat4 projMatrix,
    glm::vec3& outOrigin,
    glm::vec3& outDirection
)
{
    // Get a ray from camera but converted into NDC
    glm::vec4 rayStartNDC(
        mouseX,
        mouseY,
        //(mouseX / (float)screenWidth - 0.5f) * 2.0f,
        //(mouseY / (float)screenHeight - 0.5f) * 2.0f,
        -1.0f, // Z=-1 since near plane maps to -1 in NDC
        1.0f
    );

    glm::vec4 rayEndNDC(
        mouseX,
        mouseY,
        //(mouseX / (float)screenWidth - 0.5f) * 2.0f,
        //(mouseY / (float)screenHeight - 0.5f) * 2.0f,
        0.0f, // Z=0 for farplane in NDC
        1.0f
    );

    glm::mat4 worldSpaceMatrix = glm::inverse(projMatrix * viewMatrix);

    glm::vec4 rayStartWorld = worldSpaceMatrix * rayStartNDC;
    rayStartWorld /= rayStartWorld.w;

    glm::vec4 rayEndWorld = worldSpaceMatrix * rayEndNDC;
    rayEndWorld /= rayEndWorld.w;

    //LOG_INFO("Start = {0}, {1}, {2}, {3}", rayStartWorld.x, rayStartWorld.y, rayStartWorld.z, rayStartWorld.w);
    //LOG_INFO("End   = {0}, {1}, {2}, {3}", rayEndWorld.x, rayEndWorld.y, rayEndWorld.z, rayEndWorld.w);

    //if (editorCamera->isPerspective)
    //{
    //    rayEndWorld = worldSpaceMatrix * rayEndNDC;
    //    rayEndWorld /= rayEndWorld.w;
    //}
    //else
    //{
    //    rayEndWorld = rayStartWorld;
    //    rayEndWorld.z = rayStartWorld.z - 10000.0f;
    //}

    outOrigin = rayStartWorld;
    outDirection = glm::normalize(rayEndWorld - rayStartWorld);
}

bool EditorLayer::OnMouseClick(MouseButtonPressedEvent& e)
{
    /*
    if ((e.GetMouseButton() == MOUSE_BUTTON_LEFT))
    {
        LOG_TRACE("Event pressed");
    }
    // FIXME GetMouseButtonDown in input handler not getting refreshed?
    if (Input::GetMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        LOG_TRACE("MB pressed");
    }
    */

    if (!allowViewportCameraEvents)
    {
        return false;
    }

    // Don't raycast for an object if clicking ImGuizmo
    if ((e.GetMouseButton() == MOUSE_BUTTON_LEFT)
        && !ImGuizmo::IsOver())
    {
        auto [mouseX , mouseY] = GetMouseViewportSpace();
        glm::vec3 rayOrig, rayDir;
        ScreenToWorldRay(
            mouseX,
            mouseY,
            editorCamera->width,
            editorCamera->height,
            editorCamera->GetViewMatrix(),
            editorCamera->GetProjMatrix(),
            rayOrig,
            rayDir
        );

        wHierarchy.Reset();

        // Check all sprite objects for intersection
        float tHit = FLT_MAX;
        Entity selectedEntity {};
        auto spriteGroup = activeScene->GetAllEntitiesWith<SpriteComponent>();
        for (auto e : spriteGroup)
        {
            Entity entity { e, activeScene.get() };

            Ray ray = {
                glm::inverse(glm::mat4(entity.GetComponent<TransformComponent>())) * glm::vec4(rayOrig, 1.0f),
                glm::inverse(glm::mat3(entity.GetComponent<TransformComponent>())) * rayDir
            };

            SpriteComponent comp = entity.GetComponent<SpriteComponent>();
            float t = FLT_MAX;
            if (ray.IntersectAABB(comp.boundingBox, t))
            {
                if (t < tHit)
                {
                    tHit = t;
                    selectedEntity = entity;
                }
            }

        }

        // Check all mesh objects for intersection
        auto meshGroup = activeScene->GetAllEntitiesWith<MeshComponent>();
        for (auto e : meshGroup)
        {
            Entity entity { e, activeScene.get() };
            auto mesh = entity.GetComponent<MeshComponent>().mesh;
            // Skip if no mesh exists
            if (!mesh) { continue; }

            auto& submeshes = mesh->GetSubmeshes();
            for (auto submesh : submeshes)
            {
                Ray ray = {
                    glm::inverse(glm::mat4(entity.GetComponent<TransformComponent>())) * glm::vec4(rayOrig, 1.0f),
                    glm::inverse(glm::mat3(entity.GetComponent<TransformComponent>())) * rayDir
                };

                float t = FLT_MAX;
                if (ray.IntersectAABB(submesh.boundingBox, t))
                {
                    //LOG_TRACE("Has hit for {0}", entity.GetComponent<TagComponent>().tag);
                    //wHierarchy.OverrideSelectedEntity(entity, activeScene);
                    if (t < tHit)
                    {
                        tHit = t;
                        selectedEntity = entity;
                    }
                    // TODO check if ray intersects the triangles for more precise picking
                }
            }
        }

        if (selectedEntity)
        {
            wHierarchy.OverrideSelectedEntity(selectedEntity, activeScene);
        }
    }

    return false;
}

// TODO remove later.. just for testing FBO
bool EditorLayer::OnWindowResize(WindowResizeEvent& e)
{
    //LOG_INFO("Resize: ScrWidth = {0}, ScrHeight = {1}", e.GetWidth(), e.GetHeight());
    maxResolution = glm::ivec2(e.GetWidth(), e.GetHeight());
    editorCamera->SetProjection(e.GetWidth(), e.GetHeight());

    FramebufferSpec resizeSpec = {
        e.GetWidth(),
        e.GetHeight(),
        GL_RGBA16F,
        2
    };

    gameCamFBO->Resize(resizeSpec, true);
    editorCamFBO->Resize(resizeSpec, true);
    pingpongFBOs[0]->Resize(resizeSpec, true);
    pingpongFBOs[1]->Resize(resizeSpec, true);
    blurFBO->Resize(resizeSpec, true);

    FramebufferSpec postSpec = {
        e.GetWidth(), e.GetHeight(), GL_RGBA
    };
    postprocessFBO->Resize(postSpec, true);

    return false;
}

void EditorLayer::ShowEditorCameraSettings(bool* isOpen)
{
    ImGui::Begin("Editor Camera Settings", isOpen);

    ImGui::DragFloat3("Position", (float*)&cameraController.position);
    bool prevState = editorCamera->isPerspective;
    ImGui::Checkbox("Is Perspective", &editorCamera->isPerspective);

    ImGui::Separator();

    {
        ImGui::Text("View Matrix");
        glm::mat4 view = editorCamera->GetViewMatrix();
        for (int i = 0; i < 4; ++i)
        {
            ImGui::PushID(i);
            ImGui::DragFloat4("", (float*)&view[i]);
            ImGui::PopID();
        }
    }

    ImGui::Separator();

    {
        ImGui::Text("Projection Matrix");
        glm::mat4 proj = editorCamera->GetProjMatrix();
        for (int i = 0; i < 4; ++i)
        {
            ImGui::PushID(i);
            ImGui::DragFloat4("", (float*)&proj[i]);
            ImGui::PopID();
        }
    }

    ImGui::Separator();


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

    //for (size_t i = 0; i < clips.size(); ++i)
    //{
    //    if (clips[i].empty()) { clipName = "Blank"; }
    //    else { clipName = clips[i]; }
    //    if (ImGui::Button(clipName.c_str()))
    //    {
    //        std::string path = FileSystem::OpenFile("*.wav");
    //        if (!path.empty())
    //        {
    //            clips[i] = path;
    //        }
    //    }
    //    ImGui::SameLine();
    //    if (ImGui::Button("Play"))
    //    {
    //        AudioSystem::PlaySound(clips[i].c_str());
    //    }
    //}

    ImGui::End();
}

void EditorLayer::ShowConsole(bool* isOpen)
{
    // TODO make console widget
}

bool EditorLayer::OnWindowClose(WindowCloseEvent& e)
{
    // Signal that close prompt should be opened
    // Need to do this here since events and imgui rendering is decoupled
    // and cannot render imgui in an event
    shouldOpenExitPrompt = true;
    return false;
}

//bool EditorLayer::OnLuaSetGlobal(LuaSetGlobalEvent& e)
//{
//    return false;
//}

// TODO
bool EditorLayer::OpenConfirmPrompt(const char* msg)
{
    bool isConfirmed = false;

    ImGui::OpenPopup("Continue?");
    if (ImGui::BeginPopupModal("Continue?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text(msg);
        ImGui::Text("\n\nConfirm?\n\n");
        ImGui::Separator();

        if (ImGui::Button("Confirm", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            isConfirmed = true;
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    return isConfirmed;
}

void EditorLayer::OpenClosePrompt()
{
    // Open confirmation prompt
    ImGui::OpenPopup("Quit?");
    if (ImGui::BeginPopupModal("Quit?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Any unsaved progress will be lost!\n\nConfirm?\n\n");
        ImGui::Separator();

        if (ImGui::Button("Confirm", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            Application::Get().Close();
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            shouldOpenExitPrompt = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

std::pair<float, float> EditorLayer::GetMouseViewportSpace()
{
    auto [mouseX, mouseY] = ImGui::GetMousePos();
    mouseX -= minViewportBound.x;
    mouseY -= minViewportBound.y;

    //auto viewportWidth = maxViewportBound.x - minViewportBound.x;
    //auto viewportHeight = maxViewportBound.y - minViewportBound.y;
    //LOG_INFO("vp width = {0}, vp height = {1}", viewportWidth, viewportHeight);

    return {
        (mouseX / editorCamera->width) * 2.0f - 1.0f,
        ((mouseY / editorCamera->height) * 2.0f - 1.0f) * -1.0f // mouse.y is inverted
    };
}
