#include "Widgets/AssetBrowser.h"
#include "Core/ResourceManager.h"

#include <imgui.h>

AssetBrowser::AssetBrowser()
{
    // TODO save the last visited path into some engine meta file
    // so that browser opens back where it left off
    currPath = ASSETS;
}

void AssetBrowser::OnImGuiRender(bool* isOpen)
{
    static float scale = 1;
    ImGui::Begin("Assets", isOpen);

    // Scaling size for icons
    ImGui::SliderFloat("Icon Scale", &scale, 0.5f, 2.0f);

    ImGui::SameLine();

    if (ImGui::Button("Parent Dir"))
    {
        if (currPath != ASSETS)
        {
            currPath = currPath.parent_path();
        }
    }

    uint32_t size = 40;
    size *= scale;
    ImVec2 buttonSize(size, size);

    ImGuiStyle& style = ImGui::GetStyle();
    float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

    int buttonCount = 0;
    for (auto& p : std::filesystem::directory_iterator(currPath))
    {
        buttonCount++;
    }

    int n = 0;
    for (auto& p : std::filesystem::directory_iterator(currPath))
    {
        ImGui::PushID(n);
        std::string assetName = p.path().filename().string();
        if (ImGui::Button(assetName.c_str(), buttonSize))
        {
            if (std::filesystem::is_directory(p.path())) {
                currPath = std::filesystem::relative(p.path());
            }
        }
        float last_button_x2 = ImGui::GetItemRectMax().x;
        float next_button_x2 = last_button_x2 + style.ItemSpacing.x + buttonSize.x; // Expected position if next button was on same line
        if (n + 1 < buttonCount && next_button_x2 < window_visible_x2)
            ImGui::SameLine();
        ImGui::PopID();

        n++;
    }



    ImGui::End();
}
