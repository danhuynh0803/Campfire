#ifndef EDITOR_LAYER_H
#define EDITOR_LAYER_H

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Core/Layer.h"
#include "Scene/Scene.h"
#include "Scene/CameraController.h"

#include "Editor/Widgets/ConsoleWidget.h"
#include "Editor/Widgets/HierarchyWidget.h"
#include "Editor/Widgets/InspectorWidget.h"
#include "Editor/Widgets/TransformWidget.h"

class EditorLayer : public Layer
{
public:
    EditorLayer();
    ~EditorLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& event) override;
    bool OnWindowResize(WindowResizeEvent& e);


private:
    bool startScene = false;
    bool stopScene = true;
    void ClearScene();

    SharedPtr<Scene> activeScene;
    SharedPtr<Camera> editorCamera;
    CameraController cameraController;

    void ShowMenuFile();
    void ShowMenuWindow();

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

#endif // EDITOR_LAYER_H
