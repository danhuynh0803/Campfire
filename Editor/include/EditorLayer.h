#pragma once

#include "Core/Layer.h"
#include "Scene/Scene.h"
#include "Scene/CameraController.h"

#include "Widgets/ConsoleWidget.h"
#include "Widgets/HierarchyWidget.h"
#include "Widgets/InspectorWidget.h"
#include "Widgets/TransformWidget.h"
#include "Widgets/AssetBrowser.h"
#include "Renderer/RendererAPI.h"
#include "Events/LuaEvent.h"
#include "Events/LuaManagerEvent.h"

class EditorLayer : public Layer
{
    // For drawing submesh bounding boxes
    // move to renderer
    // FIXME misc stuff for quick testing
    SharedPtr<VertexArray> VAO;
    SharedPtr<VertexBuffer> VBO;
    SharedPtr<Shader> lineShader;
    SharedPtr<Shader> blurShader;
    SharedPtr<Shader> postprocessShader;
    float exposure = 1.0f;

    DrawMode drawMode = DrawMode::SHADED;
    bool drawSkybox = true;
    bool drawPostProcess = true;
    bool drawParticleSystems = false;

public:
    EditorLayer();
    ~EditorLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& event) override;
    virtual void OnLuaEvent(LuaEvent&) override;

    bool OnWindowResize(WindowResizeEvent& e);
    bool OnMouseClick(MouseButtonPressedEvent& e);
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnLuaSetGlobal(LuaSetGlobalEvent& e);

private:
    void ProcessUserInput();
    bool OpenConfirmPrompt(const char* msg);
    void OpenClosePrompt();
    void ClearScene();
    std::pair<float, float> GetMouseViewportSpace();

    void ShowMenuFile();
    void ShowMenuWindow();
    void ShowMenuCampfire();
    void ShowAudioSettings(bool* isOpen);
    void ShowConsole(bool* isOpen);
    void ShowEditorCameraSettings(bool* isOpen);

    void ScreenToWorldRay(
        float mouseX, float mouseY,
        int screenWidth, int screenHeight,
        glm::mat4 viewMatrix,
        glm::mat4 projMatrix,
        glm::vec3& outOrigin,
        glm::vec3& outDirection
    );

private:
    int editorSelectedIndex = -1;
    bool startScene = false;
    bool stopScene = true;
    bool shouldOpenExitPrompt = false;
    bool maximizeGameViewport = false;
    glm::ivec2 maxResolution = glm::ivec2(1920, 1080);
    SharedPtr<Scene> editorScene;
    SharedPtr<Scene> runtimeScene;
    SharedPtr<Scene> activeScene;

    bool is2d = false;
    SharedPtr<Camera> editorCamera;
    CameraController cameraController;

    // Widgets
    HierarchyWidget wHierarchy;
    InspectorWidget wInspector;
    TransformWidget wTransform;
    AssetBrowser wAssetBrowser;

    bool showAudioSettings = false;
    bool showAssetBrowser = true;
    bool showBoundingBoxes = false;
    bool showLog = true;
    bool showConsole = false;
    bool showHierarchy = true;
    bool showInspector = true;
    bool showTransformSettings = false;
    bool showEditorCameraSettings = false;
    bool allowViewportCameraEvents = false;
    bool showLightSettings = false;
    bool showLightExplorer = false;

    bool hasViewportAction = false;

    glm::vec2 minViewportBound;
    glm::vec2 maxViewportBound;

    // Editor state
    enum class State
    {
        STOP = 0,
        PLAY,
        PAUSE
    };
    State state = State::STOP;

    friend AssetBrowser;
};
