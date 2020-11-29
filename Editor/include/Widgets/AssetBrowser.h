#pragma once

#include <filesystem>

class AssetBrowser
{
public:
    AssetBrowser();
    void OnImGuiRender(bool* isOpen);

private:
    std::filesystem::path currPath;
};
