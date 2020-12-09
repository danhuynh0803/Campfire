#pragma once

#include <filesystem>

class EditorLayer;

class AssetBrowser
{
public:
    ~AssetBrowser() = default;
    AssetBrowser(EditorLayer* editor);
    void OnImGuiRender(bool* isOpen);

private:
    void OpenFile(const std::filesystem::path& filepath);
    void RecurseCurrentDir(const std::filesystem::path& path);
    void RecursivelyDisplayDirectories(std::filesystem::path dirPath);
    std::filesystem::path currPath;
    int mSelectedItem = 0;
    int mSelectedDir = 0;

    EditorLayer* mEditor = nullptr;
};
