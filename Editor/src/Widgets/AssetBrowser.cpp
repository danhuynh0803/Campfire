#include "Widgets/AssetBrowser.h"
#include "Core/ResourceManager.h"
#include "IconsFontAwesome5.h"

#include <imgui.h>

AssetBrowser::AssetBrowser()
{
    // TODO save the last visited path into some engine meta file
    // so that browser opens back where it left off
    currPath = ASSETS;
}

void AssetBrowser::RecursivelyDisplayDirectories(std::filesystem::path dirPath)
{
    for (auto& p : std::filesystem::directory_iterator(dirPath))
    {
        if (!std::filesystem::is_directory(p.path())) { continue; }

        if (ImGui::TreeNode(p.path().filename().string().c_str()))
        {
            // Update content view with selected directory
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
            {
                currPath = std::filesystem::relative(p.path());
            }

            RecursivelyDisplayDirectories(std::filesystem::relative(p.path()));

            ImGui::TreePop();
        }
    }
}

void AssetBrowser::OnImGuiRender(bool* isOpen)
{
    static float scale = 1.0f;
    static bool isList = true;
    uint32_t size = 40;

    ImGui::Begin("Assets", isOpen);

    ImGui::BeginChild("Directory", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetContentRegionAvail().y), true);
    {
        RecursivelyDisplayDirectories(std::filesystem::path(ASSETS));
    }
    ImGui::EndChild();

    //ImGui::BeginChild("Content", ImVec2(0, ImGui::GetFontSize() * 20), true);
    ImGui::BeginChild("Content", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetContentRegionAvail().y), true);

    ImGui::Checkbox("List View", &isList);

    ImGui::SameLine();

    if (ImGui::Button("Parent Dir"))
    {
        if (currPath.string() != ASSETS
            && currPath.has_parent_path())
        {
            currPath = std::filesystem::relative(currPath.parent_path());
        }
    }

    // Scaling size for icons
    ImGui::SliderFloat("Scale", &scale, 1.0f, 3.0f);
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

        if (isList)
        {
            std::filesystem::is_directory(p.path())
                ? ImGui::Text(ICON_FA_FOLDER)
                : ImGui::Text(ICON_FA_FILE)
            ;
            ImGui::SameLine();

            if (ImGui::Button(assetName.c_str()))
            {
                if (std::filesystem::is_directory(p.path())) {
                    currPath = std::filesystem::relative(p.path());
                }
                else
                {
                    // TODO open selected file
                }
            }
        }
        else
        {
            if (std::filesystem::is_directory(p.path()))
            {
                if (ImGui::Button(ICON_FA_FOLDER, buttonSize))
                {
                    currPath = std::filesystem::relative(p.path());
                }
            }
            else
            {
                auto texture = ResourceManager::GetTexture2D(p.path().string());
                ImGui::Image((ImTextureID)texture->GetRenderID(), buttonSize, ImVec2(0,1), ImVec2(1,0));
                ImGui::Text(assetName.c_str());
                //if (ImGui::ImageButton((ImTextureID)texture->GetRenderID(), buttonSize, ImVec2(0,1), ImVec2(1,0)))
                {

                }
            }
        }

        float last_button_x2 = ImGui::GetItemRectMax().x;
        float next_button_x2 = last_button_x2 + style.ItemSpacing.x + buttonSize.x; // Expected position if next button was on same line
        if (n + 1 < buttonCount
            && next_button_x2 < window_visible_x2
            && !isList
        ) {
            ImGui::SameLine();
        }
        ImGui::PopID();

        n++;
    }
    ImGui::EndChild();

    ImGui::End();
}
