#include "Scripting/Script.h"
#include "Scene/Component.h"

#include <lua.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Script::Script(Entity e)
    : entity(e)
{
}

template <typename T>
T& Script::GetComponent()
{
    return entity.GetComponent<T>();
}

//Entity Script::Instantiate(Entity entity, glm::vec3 position)
//{
    //auto newEntity = entity.scene->CreateEntity("InstObject");
    //newEntity.GetComponent<TransformComponent>().position = position;
    //newEntity.AddComponent<MeshComponent>(MeshComponent::Geometry::CUBE);

    //return newEntity;
//}

void Script::Destroy(Entity other, float timer)
{
    //entity.scene->RemoveEntity(other);
}

void Script::OnImGuiRender()
{
    std::string filename = filepath.empty() ? "Blank" : filepath;
    if (ImGui::Button(filename.c_str()))
    {
        std::string path = FileSystem::OpenFile("*.lua");
        if (!path.empty())
        {
            filepath = path;
        }
    }
}
