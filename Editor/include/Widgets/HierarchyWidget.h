#ifndef HIERARCHY_WIDGET_H
#define HIERARCHY_WIDGET_H

#include "Scene/Scene.h"
#include "Widgets/InspectorWidget.h"

class HierarchyWidget
{
public:
    void ShowHierarchy(SharedPtr<Scene>& activeScene, const SharedPtr<Camera>& editorCam, bool* isOpen);

    void Reset()
    {
        selected = -1;
        hasSelectedEntity = false;
    }

    Entity& GetSelectedEntity() { return selectedEntity; }
    bool HasSelectedEntity() { return hasSelectedEntity; }
    void OverrideSelectedIndex(int index) { selected = index; }
    int GetSelectedIndex() const { return selected; }

private:
    void ShowNewEntityMenu(SharedPtr<Scene>& activeScene, const SharedPtr<Camera>& editorCam);

    // For displaying entity properties
    Entity selectedEntity;
    InspectorWidget wInspector;
    int selected = -1;
    bool hasSelectedEntity = false;
};

#endif // HIERARCHY_WIDGET_H
