#pragma once

#include <lua.hpp>
#include "Scene/Entity.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class Script
{
public:
//    Script(Entity e)
//        : entity(e)
//    {
//    }
//
//    template <typename T>
//    T& GetComponent()
//    {
//        return entity.GetComponent<T>();
//    }
//
//    Entity Instantiate(Entity entity, glm::vec3 position)
//    {
//        auto newEntity = entity.scene->CreateEntity("InstObject");
//        newEntity.GetComponent<TransformComponent>().position = position;
//        newEntity.AddComponent<MeshComponent>(MeshComponent::Geometry::CUBE);
//
//        return newEntity;
//    }
//
//    void Destroy(Entity other, float timer = 0.0f)
//    {
//        entity.scene->RemoveEntity(other);
//    }
//
//    //void Destroy(ScriptableEntity* other, float timer = 0.0f)
//    //{
//    //    entity.scene->RemoveEntity(other->entity);
//    //}
//
    void OnImGuiRender()
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
    //
//    virtual void Start() {}
//    virtual void Update() {}
//    virtual void Update(float dt) {}
//    virtual void OnTriggerEnter(Entity other) {}
//    virtual void OnTriggerStay(Entity other) {}
//    virtual void OnTriggerExit(Entity other) {}
//    Entity entity;

private:
    std::string filepath;

    friend class Scene;
};
