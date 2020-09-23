#include <imgui.h>
#include "Editor/Widgets/HierarchyWidget.h"
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

void HierarchyWidget::ShowHierarchy(SharedPtr<Scene>& activeScene, bool* isOpen)
{
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(1.0f, 0.6f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(1.0f, 0.6f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(1.0f, 0.6f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::Begin("Scene Hierarchy", isOpen);
    ImGui::PopStyleColor(1);

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
    if (ImGui::BeginPopupContextWindow(0, 1, false))
    {
        ShowNewEntityMenu(activeScene);
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopup("NewEntityPopup"))
    {
        ShowNewEntityMenu(activeScene);
        ImGui::EndPopup();
    }

    //HelpMarker(filterHelp);
    ImGui::SameLine(); filter.Draw();

    ImGui::Separator();

    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    static int selection_mask = (1 << 2);

    int rootIdx = 0;
    static int childIdx = -1;
    for (auto entityPair : activeScene->GetEntityMap())
    {
        ImGuiTreeNodeFlags node_flags = base_flags;
        const bool is_selected = (selection_mask & (1 << rootIdx)) != 0;

        if (is_selected)
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        Entity entity = entityPair.second;
        std::string tag = entity.GetComponent<TagComponent>().tag;

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
        else
        {
            // Object contains children, so set as selectable tree
            bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)rootIdx, node_flags, "%d. %s", rootIdx, tag.c_str());
            if (ImGui::IsItemClicked())
            {
                selected = rootIdx;
                childIdx = -1;
            }
            if (node_open)
            {
                entt::entity curr = relationshipComp.first;
                for (size_t numChildren = 0; numChildren < relationshipComp.numChildren; ++numChildren)
                {
                    node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet

                    Entity childEntity(curr, activeScene.get());
                    std::string childTag = childEntity.GetComponent<TagComponent>();
                    int letterIdx = numChildren + 97;
                    ImGui::TreeNodeEx((void*)(char)letterIdx, node_flags, "%c. %s", letterIdx, childTag.c_str());
                    if (ImGui::IsItemClicked())
                    {
                        selected = rootIdx;
                        childIdx = numChildren;
                    }

                    curr = childEntity.GetComponent<RelationshipComponent>().next;
                }
                ImGui::TreePop();
            }
        }

        if (selected == rootIdx)
        {
            auto rootEntity = entityPair.second;
            if (childIdx == -1)
            {
                selectedEntity = rootEntity;
            }
            else
            {
                RelationshipComponent& relationshipComp = rootEntity.GetComponent<RelationshipComponent>();
                entt::entity curr = relationshipComp.first;
                Entity childEntity(curr, activeScene.get());
                for (size_t numChildren = 0; numChildren < childIdx; ++numChildren)
                {
                    curr = childEntity.GetComponent<RelationshipComponent>().next;
                    childEntity = Entity(curr, activeScene.get());
                    //relationshipComp = childEntity.GetComponent<RelationshipComponent>();
                }

                selectedEntity = childEntity;
            }
            hasSelectedEntity = true;
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

    //for (auto entityPair : activeScene->GetEntityMap())
    //{
    //    std::string tag = entityPair.second.GetComponent<TagComponent>().tag;
    //    if (filter.PassFilter(tag.c_str()))
    //    {
    //        char buf[128];
    //        // FIXME: adding an index into tag since duplicate tags
    //        // cause us to only be able to select the first of that matching tag
    //        sprintf(buf, "%d. %s", i, tag.c_str());
    //        if (ImGui::Selectable(buf, selected == i))
    //        {
    //            selected = i;
    //        }

    //        if (selected == i)
    //        {
    //            // Open inspector for selected object
    //            selectedEntity = entityPair.second;
    //            hasSelectedEntity = true;
    //        }
    //    }
    //    ++i;
    //}

    if (Input::GetKeyDown(KEY_DELETE) && selected != -1)
    {
        activeScene->RemoveEntity(selectedEntity);
        hasSelectedEntity = false;
    }

    if (hasSelectedEntity)
    {
       wInspector.ShowInspector(selectedEntity, isOpen);
    }
    ImGui::PopStyleColor(3);
    ImGui::End();
}

void HierarchyWidget::ShowNewEntityMenu(SharedPtr<Scene>& activeScene)
{
    if (ImGui::MenuItem("Create Empty"))
    {
        activeScene->CreateEntity("Empty");
    }

    if (ImGui::BeginMenu("3D Object"))
    {
        if (ImGui::MenuItem("Cube"))
        {
            auto entity = activeScene->CreateEntity("Cube");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::CUBE);
        }
        if (ImGui::MenuItem("Sphere"))
        {
            auto entity = activeScene->CreateEntity("Sphere");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::SPHERE);
        }
        if (ImGui::MenuItem("Plane"))
        {
            // TODO - no plane model currently, should be like a larger quad
            auto entity = activeScene->CreateEntity("Plane");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::PLANE);
        }
        if (ImGui::MenuItem("Quad"))
        {
            auto entity = activeScene->CreateEntity("Quad");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::QUAD);
        }
        if (ImGui::MenuItem("Cone"))
        {
            auto entity = activeScene->CreateEntity("Cone");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::CONE);
        }
        if (ImGui::MenuItem("Cylinder"))
        {
            auto entity = activeScene->CreateEntity("Cylinder");
            entity.AddComponent<MeshComponent>(MeshComponent::Geometry::CYLINDER);
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("2D Object"))
    {
        if (ImGui::MenuItem("Sprite"))
        {
            auto entity = activeScene->CreateEntity("Sprite");
            entity.AddComponent<SpriteComponent>();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Effects"))
    {
        if (ImGui::MenuItem("Particle System"))
        {
            auto entity = activeScene->CreateEntity("Particle System");
            entity.AddComponent<ParticleSystemComponent>();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Light"))
    {
        if (ImGui::MenuItem("Directional Light"))
        {
            auto entity = activeScene->CreateEntity("Directional Light");
            entity.AddComponent<LightComponent>(LightComponent::LightType::DIRECTIONAL);
        }
        if (ImGui::MenuItem("Point Light"))
        {
            auto entity = activeScene->CreateEntity("Point Light");
            entity.AddComponent<LightComponent>(LightComponent::LightType::POINT);
        }
        // TODO
        if (ImGui::MenuItem("Spot Light"))
        {
            auto entity = activeScene->CreateEntity("Spot Light");
            entity.AddComponent<LightComponent>(LightComponent::LightType::SPOT);
        }
        // TODO
        if (ImGui::MenuItem("Area Light"))
        {
            auto entity = activeScene->CreateEntity("Area Light");
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
            auto entity = activeScene->CreateEntity("Text");
            entity.AddComponent<TextComponent>();
        }
        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Camera"))
    {
        auto entity = activeScene->CreateEntity("Camera");
        entity.AddComponent<CameraComponent>();
    }
}
