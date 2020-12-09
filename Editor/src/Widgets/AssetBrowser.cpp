#include "Widgets/AssetBrowser.h"
#include "Core/ResourceManager.h"
#include "IconsFontAwesome5.h"
#include "Core/FileSystem.h"
#include <imgui.h>
#include <imgui_internal.h>
#include "EditorLayer.h"
#include "Scene/SceneManager.h"

static std::string icon;

static std::map<std::string, std::string> extToIconMap
{
    {".lua", ICON_FA_FILE},
    {".cf", ICON_FA_FILE},
    {".prefab", ICON_FA_FILE},
};

static std::string MapExtToIcon(std::string ext)
{
    if (extToIconMap.find(ext) != extToIconMap.end())
    {
        return extToIconMap.at(ext);
    }

    return ICON_FA_FILE;
}

AssetBrowser::AssetBrowser(EditorLayer* editor)
    : mEditor(editor)
{
    // TODO save the last visited path into some engine meta file
    // so that browser opens back where it left off
    currPath = ASSETS;
}

void AssetBrowser::OpenFile(const std::filesystem::path& path)
{
    std::string filepath = path.string();
    size_t dot = filepath.find_last_of(".");
    std::string ext;
    if (dot == std::string::npos)
    {
        return;
    }

    ext = filepath.substr(dot, filepath.size() - dot);
    // Scene files
    if (ext == ".cf")
    {
        mEditor->editorScene = mEditor->activeScene = SceneManager::LoadScene(filepath);
    }
}

void AssetBrowser::RecurseCurrentDir(const std::filesystem::path& path)
{
    // Stop when we traverse beyond the Assets dir
    if (path == std::filesystem::path(ASSETS).parent_path())
    {
        return;
    }

    RecurseCurrentDir(path.parent_path());

    if (std::filesystem::is_directory(path))
    {
        if (ImGui::Button(path.filename().string().c_str()))
        {
            currPath = path;
        }
        ImGui::SameLine(); ImGui::Text(">"); ImGui::SameLine();
    }
}

void AssetBrowser::OnImGuiRender(bool* isOpen)
{
    static float scale = 3.0f;
    static bool isList = false;
    uint32_t size = 40;

    // Left column -- displays directory list
    ImGui::Begin("Assets", isOpen);

    float contentWidth = ImGui::GetContentRegionAvail().x;
    float contentHeight = ImGui::GetContentRegionAvail().y;

    // Left column -- displays tree of dirs
    ImGui::Columns(2, "assetColumns");
    ImGui::SetColumnWidth(0, contentWidth * 0.3f);
    ImGui::BeginChild("Directory", ImGui::GetContentRegionAvail(), true);
    {
        RecursivelyDisplayDirectories(std::filesystem::path(ASSETS));
    }
    ImGui::EndChild();
    ImGui::NextColumn();
    ImGui::SameLine();

    // Right column -- displays contents of selected directory
    ImGui::BeginChild("Content", ImGui::GetContentRegionAvail(), true);
    {
        // Display list of dirs from Assets to the currently selected path
        // Recurse since its cleaner to print in reverse order using parent_path
        // instead of getting subdirs by parsing the string path
        RecurseCurrentDir(currPath);

        ImGui::Separator();

        ImGui::Checkbox("List View", &isList);
        ImGui::SameLine();
        // Scaling size for buttons
        ImGui::SliderFloat("Scale", &scale, 1.0f, 3.0f);
        ImGui::Separator();
        ImVec2 buttonSize(size * scale, size * scale);

        ImGuiStyle& style = ImGui::GetStyle();
        float windowVisibleX2 = ImGui::GetWindowPos().x + contentWidth;

        int buttonCount = 0;
        for (auto& p : std::filesystem::directory_iterator(currPath))
        {
            buttonCount++;
        }

        if (!isList)
        {
            int numColumns = ImGui::GetContentRegionAvail().x / buttonSize.x;
            numColumns = numColumns > 0 ? numColumns : 1;
            ImGui::Columns(numColumns, nullptr, false);
        }

        int n = 0;
        for (auto& p : std::filesystem::directory_iterator(currPath))
        {
            ImGui::PushID(n);
            std::string filename = p.path().filename().string();

            //if (ImGui::Selectable(filename.c_str(), n == mSelectedItem))
            //{
            //    mSelectedItem = n;
            //}
            //ImGui::SameLine();

            if (isList) // tabulated
            {
                std::string icon = std::filesystem::is_directory(p.path())
                    ? ICON_FA_FOLDER
                    : ICON_FA_FILE
                ;
                ImGui::Text(icon.c_str());
                ImGui::SameLine();
                if (ImGui::Button(filename.c_str()))
                {
                    if (std::filesystem::is_directory(p.path()))
                    {
                        currPath = std::filesystem::relative(p.path());
                    }
                    else
                    {
                        // TODO display in assetViewer
                    }
                }
            }
            else // display icons
            {
                size_t dot = filename.find_last_of(".");
                std::string ext;
                if (dot != std::string::npos)
                {
                    ext = filename.substr(dot, filename.size() - dot);
                }

                ImGui::BeginGroup();

                // Texture files
                if (
                    ext == ".jpg"
                 || ext == ".png"
                 )
                {
                    auto texture = ResourceManager::GetTexture2D(p.path().string());
                    ImGui::ImageButton((ImTextureID)texture->GetRenderID(), buttonSize, ImVec2(0, 1), ImVec2(1, 0));
                    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                    {
                        // Display in inspector
                    }
                    //IsMouseDoubleClicked and IsMouseClicked were both set to true
                    //so IsMouseDoubleClicked and !IsMouseClicked wont work
                    //The above will be invoked once before this is invoked
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                    {
                        FileSystem::OpenFileWithDefaultProgram(p.path().string().c_str());
                    }
                    if (ImGui::BeginPopupContextItem("Right Click Menu"))
                    {
                        if (ImGui::Button("Browse"))
                        {
                            FileSystem::OpenFileWithDefaultProgram(p.path().string().c_str());
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::OpenPopupOnItemClick("Right Click Menu", 1);
                }
                //scripts
                else if (ext == ".txt" || ext == ".lua" || ext == ".cpp")
                {
                    ImGui::Button(MapExtToIcon(ext).c_str(), buttonSize);
                    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                    {
                        // Display in inspector
                    }
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                    {
                        FileSystem::OpenFileWithDefaultProgram(p.path().string().c_str());
                    }
                    if (ImGui::BeginPopupContextItem("Right Click Menu"))
                    {
                        if (ImGui::Button("Open"))
                        {
                            FileSystem::OpenFileWithDefaultProgram(p.path().string().c_str());
                            ImGui::CloseCurrentPopup();
                        }
                        //if (ImGui::Button("Edit"))
                        //{
                        //    FileSystem::EditFileWithDefaultProgram(p.path().string().c_str());
                        //    ImGui::CloseCurrentPopup();
                        //}
                        ImGui::EndPopup();
                    }
                    ImGui::OpenPopupOnItemClick("Right Click Menu", 1);
                }
                // Directories
                else if (std::filesystem::is_directory(p.path()))
                {
                    if (ImGui::Button(ICON_FA_FOLDER, buttonSize))
                    {
                        // Display in inspector
                    }
                    //if the if statement is wrapped around the Button
                    //then this is actually click + double click
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                    {
                        currPath = std::filesystem::relative(p.path());
                    }

                    if (ImGui::BeginPopupContextItem("Right Click Menu"))
                    {
                        #ifdef WIN32
                            if (ImGui::Button("Open In Explorer"))
                            {
                                FileSystem::OpenInDirectory(p.path().string().c_str());
                                ImGui::CloseCurrentPopup();
                            }
                            ImGui::EndPopup();
                        #else
                            if (ImGui::Button("Open Directory"))
                            {

                                FileSystem::OpenInDirectory(p.path().string().c_str());
                                ImGui::CloseCurrentPopup();
                            }
                            ImGui::EndPopup();
                        #endif // WIN32
                    }
                    ImGui::OpenPopupOnItemClick("Right Click Menu", 1);
                }
                else
                {
                    ImGui::Button(MapExtToIcon(ext).c_str(), buttonSize);
                    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                    {
                        // Display in inspector
                    }
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                    {
                        //FileSystem::OpenFileWithDefaultProgram(p.path().string().c_str());
                        OpenFile(p.path());
                    }
                    if (ImGui::BeginPopupContextItem("Right Click Menu"))
                    {
                        if (ImGui::Button("Browse"))
                        {
                            FileSystem::OpenFileWithDefaultProgram(p.path().string().c_str());
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::OpenPopupOnItemClick("Right Click Menu", 1);
                }
                ImGui::TextWrapped(filename.c_str());
            }

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

    //static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
    //const bool is_selected = (selection_mask & (1 << i)) != 0;
    //if (is_selected)
    //    flags |= ImGuiTreeNodeFlags_Selected;

    if (!std::filesystem::is_directory(dirPath))
    {
        return;
    }

    ImGui::Text(ICON_FA_FOLDER); ImGui::SameLine();
    // Not sure why this isnt working outside the recursive function
    if (dirPath.string() == ASSETS) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    }
    bool nodeOpen = ImGui::TreeNodeEx(dirPath.filename().string().c_str(), flags);
    // Update content view ImGuiwith selected directory
    //ImGui::Selectable()

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
