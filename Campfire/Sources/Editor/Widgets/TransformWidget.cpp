#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Editor/Widgets/TransformWidget.h"

void TransformWidget::EditTransform(Entity& entity, const Camera& editorCamera)
{
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    ImGuiIO& io = ImGui::GetIO();
    // Note: numbers based on ascii table
    if (!io.WantCaptureKeyboard)
    {
        if (ImGui::IsKeyPressed(87)) // w
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(69)) // e
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(82)) // r
            mCurrentGizmoOperation = ImGuizmo::SCALE;
    }

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }

    switch (mCurrentGizmoOperation)
    {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap[0]);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap[0]);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap[0]);
            break;
    }
    ImGui::Checkbox("Bound Sizing", &boundSizing);
    if (boundSizing)
    {
        ImGui::PushID(3);
        ImGui::Checkbox("", &boundSizingSnap);
        ImGui::SameLine();
        ImGui::InputFloat3("Snap", boundsSnap);
        ImGui::PopID();
    }

    // TODO broken
    //ImGui::Checkbox("", &useSnap);
    //ImGui::SameLine();

    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    float* viewMatrix = const_cast<float*>(glm::value_ptr(editorCamera.GetViewMatrix()));
    float* projMatrix = const_cast<float*>(glm::value_ptr(editorCamera.GetProjMatrix()));

    if (!entity.HasComponent<TransformComponent>())
    {
        // Don't draw Gizmo if transform componenet not present
        return;
    }

    auto& transformComp = entity.GetComponent<TransformComponent>();
    float* transform = const_cast<float*>(glm::value_ptr(transformComp.GetTransform()));

    ImGuizmo::Manipulate(viewMatrix, projMatrix, mCurrentGizmoOperation, mCurrentGizmoMode, transform, NULL, useSnap ? &snap[0] : NULL, boundSizing?bounds:NULL, boundSizingSnap?boundsSnap:NULL);

    // Update transform component's data
    float newTranslation[3], newRotation[3], newScale[3];
    ImGuizmo::DecomposeMatrixToComponents(transform, newTranslation, newRotation, newScale);
    transformComp.position = glm::make_vec3(newTranslation);
    transformComp.rotation = glm::make_vec3(newRotation);
    transformComp.scale    = glm::make_vec3(newScale);
}

void TransformWidget::ShowTransformSettings()
{

}


