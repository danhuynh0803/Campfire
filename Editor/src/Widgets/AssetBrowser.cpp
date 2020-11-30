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

void AssetBrowser::OnImGuiRender(bool* isOpen)
{
    static float scale = 1.0f;
    static bool isList = true;
    uint32_t size = 40;

    ImGui::Begin("Assets", isOpen);
    ImGui::Columns(2, "assetColumns");
    ImGui::BeginChild("Directory", ImGui::GetContentRegionAvail(), true);
    {
        //ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        RecursivelyDisplayDirectories(std::filesystem::path(ASSETS));

        //if (ImGui::TreeNode("Assets"))
        //{
        //    RecursivelyDisplayDirectories(std::filesystem::path(ASSETS));
        //    ImGui::TreePop();
        //}
    }
    ImGui::EndChild();

    ImGui::NextColumn();
    ImGui::SameLine();

    ImGui::BeginChild("Content", ImGui::GetContentRegionAvail(), true);
    {
        ImGui::Checkbox("List View", &isList);

        ImGui::SameLine();

        // Scaling size for icons
        ImGui::SliderFloat("Scale", &scale, 1.0f, 3.0f);

        ImGui::Separator();

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
    }
    ImGui::EndChild();

    ImGui::End();
}

void AssetBrowser::RecursivelyDisplayDirectories(std::filesystem::path dirPath)
{
    static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!std::filesystem::is_directory(dirPath))
    {
        return;
    }

    bool nodeOpen = ImGui::TreeNodeEx(dirPath.filename().string().c_str(), flags);
    // Update content view ImGuiwith selected directory
    if (ImGui::IsItemClicked()
            && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x)
            > ImGui::GetTreeNodeToLabelSpacing()
       ){
        currPath = std::filesystem::relative(dirPath);
    }

    if (nodeOpen)
    {
        for (auto& p : std::filesystem::directory_iterator(dirPath))
        {
            RecursivelyDisplayDirectories(std::filesystem::relative(p.path()));
        }
        ImGui::TreePop();
    }
}
