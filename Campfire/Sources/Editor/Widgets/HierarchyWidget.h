#ifndef HIERARCHY_WIDGET_H
#define HIERARCHY_WIDGET_H

#include "Scene/Scene.h"
#include "Editor/Widgets/InspectorWidget.h"

class HierarchyWidget
{
public:
    void ShowHierarchy(SharedPtr<Scene>& activeScene, bool* isOpen);

    void Reset()
    {
        selected = -1;
        hasSelectedEntity = false;
    }

    bool hasSelectedEntity = false;
    Entity& GetSelectedEntity() { return selectedEntity; }

    void OverrideSelectedIndex(int index) { selected = index; }
private:
    void ShowNewEntityMenu(SharedPtr<Scene>& activeScene);

    // For displaying entity properties
    Entity selectedEntity;
    InspectorWidget wInspector;
    int selected = -1;
};

#endif // HIERARCHY_WIDGET_H
