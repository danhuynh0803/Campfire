#include "Core/Application.h"
#include "Core/FileSystem.h"
#include "Audio/AudioSystem.h"
#include "EditorLayer.h"

Camera editorCamera(1600, 900, 0.1f, 100.0f);

static Entity selectedEntity;

EditorLayer::EditorLayer()
    : Layer("Editor")
{
    activeScene = CreateSharedPtr<Scene>();
}

void EditorLayer::OnAttach()
{

}

void EditorLayer::OnDetach()
{

}

void EditorLayer::OnUpdate(float dt)
{
    activeScene->OnUpdate(dt);

    activeScene->OnRenderEditor(dt, editorCamera);

    //activeScene->OnRenderRuntime(dt);
}

void EditorLayer::OnImGuiRender()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ShowMenuFile();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window"))
        {
            ShowMenuWindow();
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (showAudioSettings) { ShowAudioSettings(&showAudioSettings); }
    if (showConsole) { ShowConsole(&showConsole); }
    if (showHierarchy) { ShowHierarchy(&showHierarchy); }

    // Draw Scene Hierarchy
    //activeScene->OnImGuiRender();

    //ImGui::Begin("Scene");
    //auto viewportSize = ImGui::GetContentRegionAvail();
    //ImGui::End();
}

void EditorLayer::ShowMenuFile()
{
    if (ImGui::MenuItem("Save", "Ctrl+S"))
    {

    }
    if (ImGui::MenuItem("Save As.."))
    {

    }

    ImGui::Separator();

    if (ImGui::MenuItem("Quit"))
    {
        Application::Get().Close();
    }
}

void EditorLayer::ShowMenuWindow()
{
    if (ImGui::BeginMenu("General"))
    {
        ImGui::MenuItem("Console", NULL, &showConsole);
        ImGui::MenuItem("Hierarchy", NULL, &showHierarchy);
        ImGui::MenuItem("Inspector", NULL, &showInspector);

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Rendering"))
    {
        ImGui::MenuItem("Lighting Settings", NULL, &showLightSettings);
        ImGui::MenuItem("Light Explorer", NULL, &showLightExplorer);

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Audio"))
    {
        ImGui::MenuItem("Settings", NULL, &showAudioSettings);

        ImGui::EndMenu();
    }
}

void EditorLayer::OnEvent(Event& event)
{
}

// Editor Imgui Widgets
void EditorLayer::ShowAudioSettings(bool* isOpen)
{
    static int masterVolume = 50;
    static std::vector<std::string> clips;

    ImGui::Begin("Audio System", isOpen);

    ImGui::Text("Sound Settings");
    ImGui::SliderInt("Volume", &masterVolume, 0, 100);

    ImGui::Separator();

    ImGui::Text("Clips");
    std::string clipName;

    if (ImGui::Button("Stop All Channels"))
    {
        AudioSystem::StopAllChannels();
    }

    ImGui::Separator();

    if (ImGui::Button("Add Audio Clip"))
    {
        clips.push_back("Empty");
    }

    ImGui::NewLine();

    for (size_t i = 0; i < clips.size(); ++i)
    {
        if (clips[i].empty()) { clipName = "Blank"; }
        else { clipName = clips[i]; }
        if (ImGui::Button(clipName.c_str()))
        {
            std::string path = FileSystem::OpenFile("*.wav");
            if (!path.empty())
            {
                clips[i] = path;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Play"))
        {
            AudioSystem::PlaySound(clips[i].c_str());
        }
    }

    ImGui::End();
}

void EditorLayer::ShowConsole(bool* isOpen)
{
    // TODO make console widget
}

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


void EditorLayer::ShowHierarchy(bool* isOpen)
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
    if (ImGui::Button("+"))
    {
        ImGui::OpenPopup("NewEntityPopup");
    }
    if (ImGui::BeginPopupContextWindow(0, 1, false))
    {
        ShowNewEntityMenu();
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopup("NewEntityPopup"))
    {
        ShowNewEntityMenu();
        ImGui::EndPopup();
    }

    //HelpMarker(filterHelp);
    ImGui::SameLine(); filter.Draw();

    ImGui::Separator();

    static int selected = -1;
    int i = 0;
    for (auto entityPair : activeScene->GetEntityMap())
    {
        std::string tag = entityPair.second.GetComponent<TagComponent>().tag;
        if (filter.PassFilter(tag.c_str()))
        {
            char buf[128];
            // FIXME: adding an index into tag since duplicate tags
            // cause us to only be able to select the first of that matching tag
            sprintf(buf, "%d. %s", i, tag.c_str());
            if (ImGui::Selectable(buf, selected == i))
            {
                // Open inspector for selected object
                showInspector = true;
                selected = i;
                selectedEntity = entityPair.second;
            }
        }

        ++i;
    }

    if (selected != -1)
    {
        //showInspector = true;
        ShowInspector(selectedEntity, &showInspector);
    }

    ImGui::End();
}

void EditorLayer::ShowInspector(Entity& entity, bool* isOpen)
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
            ImGui::DragFloat3("Position", (float*)&transform.position, 0.1f);
            ImGui::DragFloat3("Rotation", (float*)&transform.rotation, 0.1f);
            ImGui::DragFloat3("Scale", (float*)&transform.scale, 0.1f);

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
            auto psPtr = entity.GetComponent<ParticleSystemComponent>().ps;
            //psPtr->OnImGuiRender();

            ImGui::TreePop();
        }
        ImGui::Separator();
    }


    // Audio

    // Script

    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("ComponentMenuPopup");
    }
    if (ImGui::BeginPopup("ComponentMenuPopup"))
    {
        ShowComponentMenu();
        ImGui::EndPopup();
    }

    ImGui::End();
}

void EditorLayer::ShowNewEntityMenu()
{
    if (ImGui::MenuItem("Create Empty"))
    {
        activeScene->CreateEntity("Empty");
    }

    if (ImGui::BeginMenu("3D Object"))
    {
        if (ImGui::MenuItem("Cube"))
        {
            auto& entity = activeScene->CreateEntity("Cube");
            entity.AddComponent<MeshComponent>(Geometry::CUBE);
        }
        if (ImGui::MenuItem("Sphere"))
        {
            auto& entity = activeScene->CreateEntity("Sphere");
            entity.AddComponent<MeshComponent>(Geometry::SPHERE);
        }
        if (ImGui::MenuItem("Plane"))
        {
            auto& entity = activeScene->CreateEntity("Plane");
            entity.AddComponent<MeshComponent>(Geometry::PLANE);
        }
        if (ImGui::MenuItem("Quad"))
        {
            auto& entity = activeScene->CreateEntity("Quad");
            entity.AddComponent<MeshComponent>(Geometry::QUAD);
        }
        if (ImGui::MenuItem("Cone"))
        {
            auto& entity = activeScene->CreateEntity("Cone");
            entity.AddComponent<MeshComponent>(Geometry::CONE);
        }
        if (ImGui::MenuItem("Cylinder"))
        {
            auto& entity = activeScene->CreateEntity("Cylinder");
            entity.AddComponent<MeshComponent>(Geometry::CYLINDER);
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("2D Object"))
    {
        if (ImGui::MenuItem("Sprite"))
        {
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Effects"))
    {
        if (ImGui::MenuItem("Particle System"))
        {
        }
        ImGui::EndMenu();
    }

    // TODO
    if (ImGui::BeginMenu("Light"))
    {
        if (ImGui::MenuItem("Directional Light"))
        {
            auto& entity = activeScene->CreateEntity("Directional Light");
            entity.AddComponent<LightComponent>(LightComponent::LightType::DIRECTIONAL);
        }
        if (ImGui::MenuItem("Point Light"))
        {
            auto& entity = activeScene->CreateEntity("Point Light");
            entity.AddComponent<LightComponent>(LightComponent::LightType::POINT);
        }
        if (ImGui::MenuItem("Spot Light"))
        {
            auto& entity = activeScene->CreateEntity("Spot Light");
            entity.AddComponent<LightComponent>(LightComponent::LightType::SPOT);
        }
        if (ImGui::MenuItem("Area Light"))
        {
            auto& entity = activeScene->CreateEntity("Area Light");
            entity.AddComponent<LightComponent>(LightComponent::LightType::AREA);
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Audio"))
    {
        // Just an object with an audio component attached
        // with transform and tag
        if (ImGui::MenuItem("Audio Source"))
        {
        }
        ImGui::EndMenu();
    }

    // TODO
    if (ImGui::BeginMenu("UI"))
    {
        ImGui::EndMenu();
    }
}

void EditorLayer::ShowComponentMenu()
{
    // TODO
    if (!selectedEntity.HasComponent<AudioComponent>())
    {
        if (ImGui::MenuItem("Audio"))
        {
            //selectedEntity.AddComponent<AudioComponent>();
        }
    }

    if (ImGui::BeginMenu("Effects"))
    {
        if (!selectedEntity.HasComponent<ParticleSystemComponent>())
        {
            if (ImGui::MenuItem("Particle System"))
            {
                selectedEntity.AddComponent<ParticleSystemComponent>();
            }
        }
        ImGui::EndMenu();
    }

    if (!selectedEntity.HasComponent<MeshComponent>())
    {
        if (ImGui::MenuItem("Mesh"))
        {
            selectedEntity.AddComponent<MeshComponent>();
        }
    }

    if (ImGui::BeginMenu("Physics"))
    {
        ImGui::EndMenu();
    }

    if (!selectedEntity.HasComponent<LightComponent>())
    {
        if (ImGui::MenuItem("Light"))
        {
            selectedEntity.AddComponent<LightComponent>();
        }
    }

    if (ImGui::MenuItem("Scripts"))
    {
    }

    if (ImGui::MenuItem("New Script"))
    {
    }
}