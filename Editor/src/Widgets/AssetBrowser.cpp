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
    static float scale = 3.0f;
    static bool isList = false;
    uint32_t size = 40;

    // Left column -- displays directory list
    ImGui::Begin("Assets", isOpen);
    ImGui::Columns(2, "assetColumns");
    ImGui::BeginChild("Directory", ImGui::GetContentRegionAvail(), true);
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        RecursivelyDisplayDirectories(std::filesystem::path(ASSETS));
    }
    ImGui::EndChild();

    ImGui::NextColumn();
    ImGui::SameLine();

    // Left column -- displays contents of selected directory
    ImGui::BeginChild("Content", ImGui::GetContentRegionAvail(), true);
    {
        ImGui::Checkbox("List View", &isList);

        ImGui::SameLine();

        // Scaling size for buttons
        ImGui::SliderFloat("Scale", &scale, 1.0f, 3.0f);
        ImGui::Separator();
        ImVec2 buttonSize(size * scale, size * scale);

        float contentWidth = ImGui::GetContentRegionAvail().x;
        float contentHeight = ImGui::GetContentRegionAvail().y;

        ImGuiStyle& style = ImGui::GetStyle();
        float windowVisibleX2 = ImGui::GetWindowPos().x + contentWidth;

        int buttonCount = 0;
        for (auto& p : std::filesystem::directory_iterator(currPath))
        {
            buttonCount++;
        }

        int numColumns = contentWidth / buttonSize.x;
        ImGui::Columns(numColumns, nullptr, false);
        int n = 0;
        for (auto& p : std::filesystem::directory_iterator(currPath))
        {
            ImGui::PushID(n);

            std::string filename = p.path().filename().string();

            std::string icon = std::filesystem::is_directory(p.path())
                ? ICON_FA_FOLDER
                : ICON_FA_FILE
            ;

            if (isList)
            {
                ImGui::Text(icon.c_str());
                ImGui::SameLine();
                ImGui::Text(filename.c_str());
            }
            else // display icons
            {
                ImGui::BeginGroup();
                auto texture = ResourceManager::GetTexture2D(p.path().string());
                ImGui::Image((ImTextureID)texture->GetRenderID(), buttonSize, ImVec2(0,1), ImVec2(1,0));
                {
                }
                ImGui::TextWrapped(filename.c_str());
            }

            if (ImGui::IsMouseDoubleClicked(0)) {
                if (std::filesystem::is_directory(p.path())) {
                    currPath = std::filesystem::relative(p.path());
                }
            }
            else if (ImGui::IsMouseClicked(0))
            {
                // TODO open asset browser
            }

            //float lastButtonX2 = ImGui::GetItemRectMax().x;
            //float nextButtonX2 = lastButtonX2 + style.ItemSpacing.x + buttonSize.x; // Expected position if next button was on same line
            //if (n + 1 < buttonCount
            //    && nextButtonX2 < windowVisibleX2;
            //    && !isList
            //) {
            //    //ImGui::SameLine();
            //}
            if (!isList)
            {
                ImGui::EndGroup();
                ImGui::NextColumn();
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

    ImGui::Text(ICON_FA_FOLDER); ImGui::SameLine();
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
