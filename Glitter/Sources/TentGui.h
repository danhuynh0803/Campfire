#ifndef TENT_GUI_H
#define TENT_GUI_H

// ImGui headers
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

class TentGui
{
    bool isEnabled = 1;

    void Init();
    void Draw();
};

#endif // TENT_GUI_H
