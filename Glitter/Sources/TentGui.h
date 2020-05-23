#ifndef TENT_GUI_H
#define TENT_GUI_H

// ImGui headers
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/ImGuizmo.h"
#include "imgui/imfilebrowser.h"

#include "ObjectManager.h"
#include "Camera.h"
#include "FrameBuffer.h"
#include "GameManager.h"

#include <vector>

class TentGui
{
public:
    void Init(GLFWwindow*);

    void RenderStateButtons(GameManager&);
    void RenderGUI(ObjectManager&);

    void ShowCamera(Camera&);
    // =================================
    // Scene related
    void ShowSceneHierarchy(ObjectManager&);
    void ShowPrimitiveGenerator(ObjectManager&);
    void ShowObjects(ObjectManager&);
    void ShowGizmo(GameObject*);
    void ShowInspector(GameObject*);
    void ShowRenderPasses(const std::vector<FrameBuffer>&);
    // =================================

    void ShowMenuFile();
    void ShowFileBrowser();
    void ShowRenderPasses();
    void ShowMetrics(double);

    bool isEnabled = 1;

    void ShowMainMenuBar();

    Camera* activeCamera;

private:
    bool show_app_dockspace = false;
    bool show_app_documents = false;
    bool show_app_main_menu_bar = false;
    bool show_app_console = false;
    bool show_app_log = false;
    bool show_app_layout = false;
    bool show_app_property_editor = false;
    bool show_app_long_text = false;
    bool show_app_auto_resize = false;
    bool show_app_constrained_resize = false;
    bool show_app_simple_overlay = false;
    bool show_app_window_titles = false;
    bool show_app_custom_rendering = false;

    bool show_app_metrics = true;
    bool show_app_style_editor = false;
    bool show_app_about = false;
};

#endif // TENT_GUI_H
