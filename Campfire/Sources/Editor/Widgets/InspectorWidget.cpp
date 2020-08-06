#include <imgui.h>
#include "Editor/Widgets/InspectorWidget.h"
#include "Core/FileSystem.h"

void InspectorWidget::ShowInspector(Entity& entity, bool* isOpen)
{
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
    if (entity.HasComponent<ParticleSystemComponent>())
    {
    }

    // Script
    if (entity.HasComponent<ScriptComponent>())
    {
    }

    ImGui::NewLine();
    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("ComponentMenuPopup");
    }
    if (ImGui::BeginPopup("ComponentMenuPopup"))
    {
        ShowComponentMenu(entity);
        ImGui::EndPopup();
    }

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

    if (!entity.HasComponent<MeshComponent>())
    {
        if (ImGui::MenuItem("Mesh"))
        {
            entity.AddComponent<MeshComponent>();
        }
    }

    if (ImGui::BeginMenu("Physics"))
    {
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


