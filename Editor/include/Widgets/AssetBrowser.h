#pragma once

#include <filesystem>

class AssetBrowser
{
public:
    AssetBrowser();
    void OnImGuiRender(bool* isOpen);

private:
    void RecursivelyDisplayDirectories(std::filesystem::path dirPath);
    std::filesystem::path currPath;
    int mSelectedItem = 0;
    int mSelectedDir = 0;
};
