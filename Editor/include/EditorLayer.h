#pragma once

#include "Core/Layer.h"
#include "Scene/Scene.h"
#include "Scene/CameraController.h"

#include "Widgets/ConsoleWidget.h"
#include "Widgets/HierarchyWidget.h"
#include "Widgets/InspectorWidget.h"
#include "Widgets/TransformWidget.h"

class EditorLayer : public Layer
{
    // For drawing submesh bounding boxes
    // move to renderer
    SharedPtr<VertexArray> VAO;
    SharedPtr<VertexBuffer> VBO;
    SharedPtr<Shader> lineShader;

public:
    EditorLayer();
    ~EditorLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& event) override;

    bool OnWindowResize(WindowResizeEvent& e);
    bool OnMouseClick(MouseButtonEvent& e);
    bool OnWindowClose(WindowCloseEvent& e);

private:
    void OnRenderEditor();
    void OnRenderRuntime();
    bool shouldOpenExitPrompt = false;
    bool OpenConfirmPrompt(const char* msg);
    void OpenClosePrompt();

private:
    int editorSelectedIndex = -1;
    bool startScene = false;
    bool stopScene = true;
    void ClearScene();

    SharedPtr<Scene> editorScene;
    SharedPtr<Scene> runtimeScene;
    SharedPtr<Scene> activeScene;

    SharedPtr<Camera> editorCamera;
    CameraController cameraController;

    void ShowMenuFile();
    void ShowMenuWindow();
    void ShowMenuCampfire();

    // Widgets
    HierarchyWidget wHierarchy;
    InspectorWidget wInspector;
    TransformWidget wTransform;

    // TODO convert to widget
    void ShowAudioSettings(bool* isOpen);
    // TODO make console widget
    void ShowConsole(bool* isOpen);
    void ShowEditorCameraSettings(bool* isOpen);

    void ShowNewEntityMenu();

    // Audio
    bool showAudioSettings = false;
    // General
    bool showLog = true;
    bool showConsole = false;
    bool showHierarchy = true;
    bool showInspector = true;
    bool showTransformSettings = false;
    bool showEditorCameraSettings = false;
    bool allowViewportCameraEvents = false;

    // Rendering
    bool showLightSettings = false;
    bool showLightExplorer = false;

    // Editor state
    enum class State
    {
        STOP = 0,
        PLAY,
        PAUSE
    };
    State state = State::STOP;
};
