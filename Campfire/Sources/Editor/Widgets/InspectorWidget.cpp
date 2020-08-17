#include <imgui.h>
#include "Editor/Widgets/InspectorWidget.h"
#include "Core/FileSystem.h"

void InspectorWidget::ShowInspector(Entity& entity, bool* isOpen)
{
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.6f,0.2f,1.0f,1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.6f, 0.2f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.6f, 0.2f, 1.0f, 1.0f));
    ImGui::Begin("Inspector", isOpen);
    // Tag
    if (entity.HasComponent<TagComponent>())
    {
        char tag[128];
        strcpy(tag, entity.GetComponent<TagComponent>().tag.c_str());
        ImGui::InputText("", tag, IM_ARRAYSIZE(tag));
        entity.GetComponent<TagComponent>().tag.assign(tag);

        ImGui::Separator();
    }

    // Transform
    if (entity.HasComponent<TransformComponent>())
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Transform"))
        {
            auto& transform = entity.GetComponent<TransformComponent>();
            ImGui::DragFloat3("Position", (float*)&transform.position, 0.01f);
            ImGui::DragFloat3("Rotation", (float*)&transform.rotation, 0.01f);
            ImGui::DragFloat3("Scale", (float*)&transform.scale, 0.01f);
            //if (ImGui::Button("..."))
            //{
            //    OpenComponentOptionsMenu();
            //}
            if (ImGui::Button("..."))
            {
                ImGui::OpenPopup("ComponentOptionsPopup");
            }
            if (ImGui::BeginPopup("ComponentOptionsPopup"))
            {
                ShowComponentOptionsMenu();
                ImGui::EndPopup();
            }

            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    // Camera
    if (entity.HasComponent<CameraComponent>())
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Camera"))
        {
            auto& camera = entity.GetComponent<CameraComponent>().camera;
            bool prevState = camera->isPerspective;
            ImGui::Checkbox("Is Perspective", &camera->isPerspective);
            if (prevState != camera->isPerspective)
                camera->SetProjection();

            if (camera->isPerspective)
                ImGui::DragFloat("Vertical FOV", &camera->vFov, 0.1f);
            else
                ImGui::DragFloat("Size", &camera->size, 0.1f);

            ImGui::DragFloat("Near Plane", &camera->nearPlane, 0.1f);
            ImGui::DragFloat("Far Plane", &camera->farPlane, 0.1f);
            ImGui::DragFloat("Depth", &camera->depth, 0.1f);

            ImGui::Text("Viewport Rect");
            ImGui::DragFloat("x", &camera->x, 0.1f);
            ImGui::DragFloat("y", &camera->y, 0.1f);
            ImGui::DragFloat("w", &camera->width, 0.1f);
            ImGui::DragFloat("h", &camera->height, 0.1f);

            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    // Mesh
    if (entity.HasComponent<MeshComponent>())
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Mesh"))
        {
            auto& mesh = entity.GetComponent<MeshComponent>().mesh;
            if (ImGui::Button("Load Mesh"))
            {
                std::string newPath = FileSystem::OpenFile();
                if (!newPath.empty())
                {
                    mesh = Mesh::Create(newPath);
                }
            }

            ImGui::SameLine();

            if (mesh)
            {
                ImGui::Text(mesh->GetName().c_str());
            }
            else
            {
                ImGui::Text("Empty Mesh");
            }

            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    // Sprite
    if (entity.HasComponent<SpriteComponent>())
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Sprite"))
        {
            auto& comp = entity.GetComponent<SpriteComponent>();
            if (ImGui::ImageButton((ImTextureID)comp.sprite->GetRenderID(), ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0), -1, ImVec4(0,0,0,0), ImVec4(0.9, 0.9f, 0.9f, 1.0f)))
            {
                std::string path = FileSystem::OpenFile();
                if (!path.empty())
                {
                    comp.sprite = Texture2D::Create(path);
                }
            }
            ImGui::ColorEdit4("Color", (float*)&comp.color);

            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    // Collider
    if (entity.HasComponent<ColliderComponent>())
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        auto& colliderComponent = entity.GetComponent<ColliderComponent>();
        if (ImGui::TreeNode(colliderComponent.GetShapeTypeString().c_str()))
        {
            auto& collider = colliderComponent.collider;
            ImGui::Checkbox("Is Trigger", &collider->isTrigger);
            collider->ShowData();
            ImGui::DragFloat3("Center", (float*)&collider->center);
            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    // Rigidbody
    if (entity.HasComponent<RigidbodyComponent>())
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Rigidbody"))
        {
            auto& rigidbody = entity.GetComponent<RigidbodyComponent>().rigidbody;

            ImGui::DragFloat("Mass", &rigidbody->mass, 0.1f);
            ImGui::DragFloat("Drag", &rigidbody->drag, 0.1f);
            ImGui::DragFloat("Angular Drag", &rigidbody->angularDrag, 0.1f);
            ImGui::Checkbox("Use Gravity", &rigidbody->useGravity);
            ImGui::Checkbox("Is Dynamic", &rigidbody->isDynamic);
            //ImGui::CheckBox("Is Kinematic", &rigidbody->isKinematic);

            ImGui::PushID(0);
            ImGui::Text("Freeze Position");
            ImGui::SameLine();
            ImGui::Checkbox("X", &rigidbody->freezePosition[0]);
            ImGui::SameLine();
            ImGui::Checkbox("Y", &rigidbody->freezePosition[1]);
            ImGui::SameLine();
            ImGui::Checkbox("Z", &rigidbody->freezePosition[2]);
            ImGui::PopID();

            ImGui::PushID(1);
            ImGui::Text("Freeze Rotation");
            ImGui::SameLine();
            ImGui::Checkbox("X", &rigidbody->freezeRotation[0]);
            ImGui::SameLine();
            ImGui::Checkbox("Y", &rigidbody->freezeRotation[1]);
            ImGui::SameLine();
            ImGui::Checkbox("Z", &rigidbody->freezeRotation[2]);
            ImGui::PopID();

            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    // Light
    if (entity.HasComponent<LightComponent>())
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Light"))
        {
            auto& color = entity.GetComponent<LightComponent>().color;

            ImGui::ColorEdit4("Light Color", (float*)&color);
            // TODO reorganize this with range, type, etc
            //ImGui::DragFloat("Range", &light.linear, 0.01f);
            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    // Particle System
    if (entity.HasComponent<ParticleSystemComponent>())
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Particle System"))
        {
            auto& psPtr = entity.GetComponent<ParticleSystemComponent>().ps;
            psPtr->OnImGuiRender();

            ImGui::TreePop();
        }
        ImGui::Separator();
    }


    // Audio
    if (entity.HasComponent<AudioComponent>())
    {
    }

    // Script
    if (entity.HasComponent<ScriptComponent>())
    {
    }

    ImGui::NewLine();
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("ComponentMenuPopup");
    }
    ImGui::PopStyleColor(1);
    if (ImGui::BeginPopup("ComponentMenuPopup"))
    {
        ShowComponentMenu(entity);
        ImGui::EndPopup();
    }
    ImGui::PopStyleColor(3);
    ImGui::End();
}

void InspectorWidget::ShowComponentMenu(Entity& entity)
{
    // TODO
    if (!entity.HasComponent<AudioComponent>())
    {
        if (ImGui::MenuItem("Audio"))
        {
            //entity.AddComponent<AudioComponent>();
        }
    }

    if (ImGui::BeginMenu("Effects"))
    {
        if (!entity.HasComponent<ParticleSystemComponent>())
        {
            if (ImGui::MenuItem("Particle System"))
            {
                entity.AddComponent<ParticleSystemComponent>();
            }
        }
        ImGui::EndMenu();
    }

    // The object should either be a sprite or a mesh
    if (!entity.HasComponent<MeshComponent>() && !entity.HasComponent<SpriteComponent>())
    {
        if (ImGui::MenuItem("Mesh"))
        {
            entity.AddComponent<MeshComponent>();
        }
        else if (ImGui::MenuItem("Sprite"))
        {
            entity.AddComponent<SpriteComponent>();
        }
    }

    if (ImGui::BeginMenu("Physics"))
    {
        if (!entity.HasComponent<RigidbodyComponent>())
        {
            if (ImGui::MenuItem("Rigidbody"))
            {
                entity.AddComponent<RigidbodyComponent>();
            }
        }
        if (!entity.HasComponent<ColliderComponent>())
        {
            if (ImGui::MenuItem("Box Collider"))
            {
                entity.AddComponent<ColliderComponent>(ColliderComponent::Shape::Box);
            }
            if (ImGui::MenuItem("Sphere Collider"))
            {
                entity.AddComponent<ColliderComponent>(ColliderComponent::Shape::Sphere);
            }
            if (ImGui::MenuItem("Capsule Collider"))
            {
                entity.AddComponent<ColliderComponent>(ColliderComponent::Shape::Capsule);
            }
        }
        ImGui::EndMenu();
    }

    if (!entity.HasComponent<LightComponent>())
    {
        if (ImGui::MenuItem("Light"))
        {
            entity.AddComponent<LightComponent>();
        }
    }

    if (ImGui::MenuItem("Scripts"))
    {
    }

    if (ImGui::MenuItem("New Script"))
    {
    }
}

void InspectorWidget::ShowComponentOptionsMenu()
{
    if (ImGui::MenuItem("Reset"))
    {
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Remove Component"))
    {
    }
    if (ImGui::MenuItem("Copy Component"))
    {
    }

    ImGui::Separator();

    // Component specific options
}


