#include "Scripting/Script.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void Script::OnImGuiRender()
{
    std::string filename = filepath.empty() ? "Blank" : filepath;
    if (ImGui::Button(filename.c_str()))
    {
        std::string path = FileSystem::OpenFile("*.lua");
        if (!path.empty())
        {
            filepath = path;
        }
    }
}
