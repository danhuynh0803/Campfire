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
    ImGui::Begin("Assets", isOpen);

    ImGui::End();
}
