#ifndef TRANSFORM_WIDGET_H
#define TRANSFORM_WIDGET_H

#include <ImGuizmo.h>
#include "Renderer/Camera.h"
#include "Scene/Entity.h"

class TransformWidget
{
public:
    void EditTransform(Entity& entity, const Camera& editorCamera);

    void ShowTransformSettings();

private:
    bool useSnap = false;
    ImGuizmo::OPERATION operation;
    ImGuizmo::MODE mode;
};

#endif // TRANSFORM_WIDGET_H

