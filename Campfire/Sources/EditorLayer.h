#ifndef EDITOR_LAYER_H
#define EDITOR_LAYER_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Core/Layer.h"

#include "Scene/Scene.h"

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
    SharedPtr<Scene> currentScene;

    void ShowMenuFile();
    void ShowMenuWindow();

    void ShowAudioSettings(bool* isOpen);
    void ShowConsole(bool* isOpen);
    void ShowHierarchy(bool* isOpen);
    void ShowInspector(Entity& entity, bool* isOpen);
    void ShowComponentMenu();

    // Audio
    bool showAudioSettings = false;
    // General
    bool showConsole = false;
    bool showHierarchy = false;
    bool showInspector = false;

    // Rendering
    bool showLightSettings = false;
    bool showLightExplorer = false;

};

#endif // EDITOR_LAYER_H
