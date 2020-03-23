#ifndef TENT_GUI_H
#define TENT_GUI_H

// ImGui headers
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "ObjectManager.h"
#include "Camera.h"
#include "FrameBuffer.h"

#include <vector>

class TentGui
{
public:
    void Init(GLFWwindow*);

    void RenderStateButtons();
    void RenderGUI(ObjectManager&);

    void ShowCamera(Camera&);
    // =================================
    // Scene related
    void ShowSceneHierarchy(ObjectManager&);
    void ShowPrimitiveGenerator(ObjectManager&);
    void ShowObjects(ObjectManager&);
    void ShowInspector(GlObject*);
    void ShowRenderPasses(const std::vector<FrameBuffer>&);
    // =================================

    void ShowRenderPasses();
    void ShowMetrics();

    bool isEnabled = 1;

};

#endif // TENT_GUI_H
