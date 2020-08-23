#ifndef INSPECTOR_WIDGET_H
#define INSPECTOR_WIDGET_H

#include "Scene/Component.h"
#include "Scene/Entity.h"

class InspectorWidget
{
public:
    void ShowInspector(Entity& entity, bool* isOpen);

private:
    void ShowComponentMenu(Entity& entity);

    template <typename T>
    void ShowComponentOptionsMenu(Entity& entity);

};

#endif // INSPECTOR_WIDGET_H
