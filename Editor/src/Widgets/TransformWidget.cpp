#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include "Widgets/TransformWidget.h"
#include "Core/Input.h"
#include "Command/CommandManager.h"
#include "Scene/Component.h"

enum MOVETYPE
{
    NONE,
    MOVE_X,
    MOVE_Y,
    MOVE_Z,
    MOVE_YZ,
    MOVE_ZX,
    MOVE_XY,
    MOVE_SCREEN,
    ROTATE_X,
    ROTATE_Y,
    ROTATE_Z,
    ROTATE_SCREEN,
    SCALE_X,
    SCALE_Y,
    SCALE_Z,
    SCALE_XYZ
};


void TransformWidget::UpdateViewport(uint32_t width, uint32_t height)
{
    ImGuizmo::SetRect(0, 0, width, height);
}

void TransformWidget::EditTransform(Entity& entity, const Camera& editorCamera)
{

    if (!entity.HasComponent<TransformComponent>())
    {
        // Don't draw Gizmo if transform componenet not present
        return;
    }
    
    ImGuiIO io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard && !Input::GetMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        if (Input::GetKeyDown(KEY_W))
            operation = ImGuizmo::TRANSLATE;
        if (Input::GetKeyDown(KEY_E))
            operation = ImGuizmo::ROTATE;
        if (Input::GetKeyDown(KEY_R))
            operation = ImGuizmo::SCALE;
    }

    // Use a separate bool instead of "useSnap" directly as this allows
    // snap to be toggled with a hotkey and/or turned on/off in the tool's settings
    bool isSnapOn = useSnap;
    if (Input::GetMod(MOD_KEY_CONTROL)) { isSnapOn = true; }

    float* viewMatrix = const_cast<float*>(glm::value_ptr(editorCamera.GetViewMatrix()));
    float* projMatrix = const_cast<float*>(glm::value_ptr(editorCamera.GetProjMatrix()));

    auto& transformComp = entity.GetComponent<TransformComponent>();
    glm::mat4& transformMat = entity.GetComponent<TransformComponent>();
    float* transform = const_cast<float*>(glm::value_ptr(transformMat));

    int moveType;
    //this method was slightly modified so that we can query out the MOVETYPE(enum)
    ImGuizmo::ManipulateWithMoveType(
        viewMatrix,
        projMatrix,
        operation,
        mode,
        transform,
        moveType,
        nullptr,                                // delta matrix
        isSnapOn ? &snap[0] : nullptr,          // snap
        boundSizing ? bounds : NULL,            // local bounds
        boundSizingSnap ? boundsSnap : NULL     // bounds snap
    );
    
    //Skip the remaing call if there is no action
    //The second expression is to make sure that the remaining calls will be skipped,
    //when a scene object is selected(gContext.mbEnable is true and Guizmo is drawn)
    //and yet there is no interaction(when we interact with inspector,etc )
    if (!ImGuizmo::IsOver || moveType == MOVETYPE::NONE) return;
    static bool wasInUse = false;
    static glm::vec3 position;//private with nonstatic also works

    //newRotation[3], newScale[3];
    //ImGuizmo::DecomposeMatrixToComponents(transform, newTranslation, newRotation,newScale);
    
    if (moveType == MOVETYPE::MOVE_X || moveType == MOVETYPE::MOVE_Y || moveType == MOVETYPE::MOVE_Z)
    {
        float newTranslation[3];
        //I added this one
        ImGuizmo::MapOutTranslationComponentFromMatrix(transform, newTranslation);
        if (!wasInUse && ImGuizmo::IsUsing() )
        {
            wasInUse = true;
            position.x = transformComp.position.x;
            position.y = transformComp.position.y;
            position.z = transformComp.position.z;
        }
        if (wasInUse && !ImGuizmo::IsUsing())
        {
            float x = position.x;
            float y = position.y;
            float z = position.z;
            wasInUse = false;
            CommandManager::Execute(std::make_unique<ActionCommand>(
                [&transformComp, newTranslation]() {transformComp.position = glm::make_vec3(newTranslation); },
                [&transformComp, x,y,z]() {transformComp.position = glm::vec3(x, y, z); }));
            return;
        }
        transformComp.position = glm::make_vec3(newTranslation);
    }
}

void TransformWidget::ShowTransformSettings(bool* isOpen)
{
    ImGui::Begin("Move Tool", isOpen);

    ImGui::NewLine();

    if (operation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mode == ImGuizmo::LOCAL))
            mode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mode == ImGuizmo::WORLD))
            mode = ImGuizmo::WORLD;
    }

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    ImGui::Text("Snap Settings (Enable with CTRL)");
    ImGui::Checkbox("", &useSnap);
    ImGui::SameLine();
    switch (operation)
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

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    ImGui::Text("Bound Size Settings");
    ImGui::Checkbox("Enable Bound Sizing", &boundSizing);
    if (boundSizing)
    {
        ImGui::PushID(3);
        ImGui::Checkbox("", &boundSizingSnap);
        ImGui::SameLine();
        ImGui::InputFloat3("Snap", boundsSnap);
        ImGui::PopID();
    }

    ImGui::End();
}


