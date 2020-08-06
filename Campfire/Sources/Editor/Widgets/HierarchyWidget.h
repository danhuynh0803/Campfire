#ifndef HIERARCHY_WIDGET_H
#define HIERARCHY_WIDGET_H

#include "Scene/Scene.h"
#include "Editor/Widgets/InspectorWidget.h"

class HierarchyWidget
{
public:
    void ShowHierarchy(SharedPtr<Scene>& activeScene, bool* isOpen);

private:
    void ShowNewEntityMenu(SharedPtr<Scene>& activeScene);

    // For displaying entity properties
    Entity selectedEntity;
    InspectorWidget wInspector;
};

#endif // HIERARCHY_WIDGET_H
