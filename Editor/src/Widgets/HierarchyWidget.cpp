#include <imgui.h>
#include "Widgets/HierarchyWidget.h"
#include "Core/Input.h"

// TODO move to general header
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void HierarchyWidget::ShowHierarchy(SharedPtr<Scene>& activeScene, const SharedPtr<Camera>& editorCam, bool* isOpen)
{
    ImGui::Begin("Scene Hierarchy", isOpen);

    // TODO cleanup later
    static ImGuiTextFilter filter;
    const char* filterHelp(
                "Filter usage:\n"
                "  \"\"         display all lines\n"
                "  \"xxx\"      display lines containing \"xxx\"\n"
                "  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
                "  \"-xxx\"     hide lines containing \"xxx\""
            );

    // Create new entity
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
    if (ImGui::Button("+"))
    {
        ImGui::OpenPopup("NewEntityPopup");
    }
    ImGui::PopStyleColor(1);

    // Also open New Entity menu when right clicking into hierarchy
    if (ImGui::BeginPopupContextWindow(0, 1, false)
        || ImGui::BeginPopup("NewEntityPopup"))
    {
        ShowNewEntityMenu(activeScene, editorCam);
        ImGui::EndPopup();
    }

    //HelpMarker(filterHelp);
    ImGui::SameLine(); filter.Draw();

    ImGui::Separator();

    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    static int selection_mask = (1 << 2);

    int rootIdx = 0;
    static int childIdx = -1;
    auto entityMap = activeScene->GetEntityMap();
    // TODO
    // replace entityMap with just querying entities directly from the registry
    // though this might mess up the ordering
    for (auto entityPair : entityMap)
    {
        ImGuiTreeNodeFlags node_flags = base_flags;
        const bool is_selected = (selection_mask & (1 << rootIdx)) != 0;

        if (is_selected)
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        Entity entity = entityPair.second;
        if (!entity) {
            continue;
        }

        std::string tag = entity.GetComponent<TagComponent>().tag;
        // Skip entities that don't match the filter
        if (filter.PassFilter(tag.c_str()))
        {
            RelationshipComponent& relationshipComp = entity.GetComponent<RelationshipComponent>();
            if (relationshipComp.numChildren == 0)
            {
                // Object contains no children, so set as selectable leaf
                node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                ImGui::TreeNodeEx((void*)(intptr_t)rootIdx, node_flags, "%d. %s", rootIdx, tag.c_str());
                if (ImGui::IsItemClicked())
                {
                    selected = rootIdx;
                    childIdx = -1;
                }
            }

            if (selected == rootIdx)
            {
                OverrideSelectedEntity(entity, activeScene);
            }
        }

        ++rootIdx;
    }

    if (selected != -1)
    {
        // Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
        if (ImGui::GetIO().KeyCtrl)
            selection_mask ^= (1 << selected);          // CTRL+click to toggle
        else //if (!(selection_mask & (1 << selected))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
            selection_mask = (1 << selected);           // Click to single-select
    }

    if (ImGui::IsWindowFocused())
    {
        if (Input::GetKeyDown(KEY_UP))
        {
            selected--;
            selected = selected < 0 ? 0 : selected;
        }
        else if (Input::GetKeyDown(KEY_DOWN))
        {
            selected++;
            selected = selected >= entityMap.size() ? entityMap.size() - 1 : selected;
        }
    }

    if (selectedEntity && Input::GetMod(MOD_KEY_CONTROL) && Input::GetKeyDown(KEY_D))
    {
        activeScene->DuplicateEntity(selectedEntity);
    }

    //if (selectedEntity)
    //{
    //    wInspector.ShowInspector(selectedEntity, isOpen);
    //}

    if (Input::GetKeyDown(KEY_DELETE) && selectedEntity)
    {
        activeScene->RemoveEntity(selectedEntity);
        auto numEntities = activeScene->GetEntityMap().size();

        if (numEntities == 0)
        {
            selected = -1;
        }
        // Deleted entry was the last object
        else if (selected == numEntities)
        {
            // Select the new last object now
            selected--;
        }
        selectedEntity = Entity{};
    }

    ImGui::End();
}

void HierarchyWidget::ShowNewEntityMenu(SharedPtr<Scene>& activeScene, const SharedPtr<Camera>& editorCam)
{
    // Instantiate new object in front of camera
    glm::mat4 view = editorCam->GetViewMatrix();
    glm::vec3 front =
        glm::vec3(
            view[0][2],
            view[1][2],
            view[2][2]
        );
    // Negative since camera faces in the opposite direction of front
    front *= -1;
    // TODO maybe scale distance by zoom?
    float distance = 7.0f;
    glm::vec3 pos = editorCam->pos + (front * distance);

    Entity entity {};

    if (ImGui::MenuItem("Create Empty"))
    {
        entity = activeScene->CreateEntity("Empty");
    }

    if (ImGui::BeginMenu("3D Object"))
    {
        if (ImGui::MenuItem("Cube"))
        {
            entity = activeScene->CreateEntity("Cube");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::CUBE);
        }
        if (ImGui::MenuItem("Sphere"))
        {
            entity = activeScene->CreateEntity("Sphere");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::SPHERE);
        }
        if (ImGui::MenuItem("Plane"))
        {
            // TODO - no plane model currently, should be like a larger quad
            entity = activeScene->CreateEntity("Plane");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::PLANE);
        }
        if (ImGui::MenuItem("Quad"))
        {
            entity = activeScene->CreateEntity("Quad");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::QUAD);
        }
        if (ImGui::MenuItem("Cone"))
        {
            entity = activeScene->CreateEntity("Cone");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::CONE);
        }
        if (ImGui::MenuItem("Cylinder"))
        {
            entity = activeScene->CreateEntity("Cylinder");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::CYLINDER);
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("2D Object"))
    {
        if (ImGui::MenuItem("Sprite"))
        {
            entity = activeScene->CreateEntity("Sprite");
            entity.AddComponent<SpriteComponent>();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Effects"))
    {
        if (ImGui::MenuItem("Particle System"))
        {
            entity = activeScene->CreateEntity("Particle System");
            entity.AddComponent<ParticleSystemComponent>();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Light"))
    {
        if (ImGui::MenuItem("Directional Light"))
        {
            entity = activeScene->CreateEntity("Directional Light");
            entity.AddComponent<LightComponent>(LightComponent::LightType::DIRECTIONAL);
        }
        if (ImGui::MenuItem("Point Light"))
        {
            entity = activeScene->CreateEntity("Point Light");
            entity.AddComponent<LightComponent>(LightComponent::LightType::POINT);
        }
        // TODO
        if (ImGui::MenuItem("Spot Light"))
        {
            entity = activeScene->CreateEntity("Spot Light");
            entity.AddComponent<LightComponent>(LightComponent::LightType::SPOT);
        }
        // TODO
        if (ImGui::MenuItem("Area Light"))
        {
            entity = activeScene->CreateEntity("Area Light");
            entity.AddComponent<LightComponent>(LightComponent::LightType::AREA);
        }

        ImGui::EndMenu();
    }

    // TODO
    if (ImGui::BeginMenu("Audio"))
    {
        // Just an object with an audio component attached
        // with transform and tag
        if (ImGui::MenuItem("Audio Source"))
        {
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("UI"))
    {
        if (ImGui::MenuItem("Text"))
        {
            entity = activeScene->CreateEntity("Text");
            entity.AddComponent<TextComponent>();
        }
        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Camera"))
    {
        entity = activeScene->CreateEntity("Camera");
        entity.AddComponent<CameraComponent>();
    }

    if (entity)
    {
        entity.GetComponent<TransformComponent>().position = pos;
        OverrideSelectedEntity(entity, activeScene);
    }
}

void HierarchyWidget::OverrideSelectedEntity(Entity overrideEntity, const SharedPtr<Scene>& activeScene)
{
    int index = 0;
    for (auto entityPair : activeScene->GetEntityMap())
    {
        //uint64_t entityID = entityPair.second.GetComponent<IDComponent>();
        entt::entity currHandle = entityPair.second;
        if (overrideEntity == currHandle)
        {
            selectedEntity = overrideEntity;
            selected = index;
            break;
        }
        index++;
    }
}
