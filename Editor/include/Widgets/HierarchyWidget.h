#pragma once

#include "Scene/Scene.h"
#include "Widgets/InspectorWidget.h"

class HierarchyWidget
{
public:
    void ShowHierarchy(SharedPtr<Scene>& activeScene, const SharedPtr<Camera>& editorCam, bool* isOpen);

    void Reset()
    {
        selectedEntity = Entity{};
        selected = -1;
    }

    Entity& GetSelectedEntity() { return selectedEntity; }
    void OverrideSelectedIndex(int index) { selected = index; }
    void OverrideSelectedEntity(Entity selectedEntity, const SharedPtr<Scene>& activeScene);
    int GetSelectedIndex() const { return selected; }

private:
    void ShowNewEntityMenu(SharedPtr<Scene>& activeScene, const SharedPtr<Camera>& editorCam);

    // For displaying entity properties
    Entity selectedEntity;
    InspectorWidget wInspector;
    int selected = -1;
};
