#ifndef EDITOR_LAYER_H
#define EDITOR_LAYER_H

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Core/Layer.h"
#include "Scene/Scene.h"

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

private:
    SharedPtr<Scene> activeScene;

    void ShowMenuFile();
    void ShowMenuWindow();

    // Widgets
    HierarchyWidget wHierarchy;
    InspectorWidget wInspector;

    // TODO convert to widget
    void ShowAudioSettings(bool* isOpen);
    void ShowConsole(bool* isOpen);

    void ShowNewEntityMenu();

    // Audio
    bool showAudioSettings = false;
    // General
    bool showConsole = false;
    bool showHierarchy = true;
    bool showInspector = true;

    // Rendering
    bool showLightSettings = false;
    bool showLightExplorer = false;

    //Camera editorCamera(1600, 900, 0.1f, 100f);
};

#endif // EDITOR_LAYER_H
