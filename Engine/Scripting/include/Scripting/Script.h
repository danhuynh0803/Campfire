#pragma once

#include <glm/glm.hpp>
#include "Scene/Entity.h"

class Script
{
public:
    Script() = default;
    Script(Entity e);
    template <typename T>
    T& GetComponent();
    Entity Instantiate(Entity entity, glm::vec3 position = glm::vec3());
    void Destroy(Entity other, float timer = 0.0f);

    void OnImGuiRender();
    virtual void Start() {}
    virtual void Update() {}
    virtual void Update(float dt) {}
    virtual void OnTriggerEnter(Entity other) {}
    virtual void OnTriggerStay(Entity other) {}
    virtual void OnTriggerExit(Entity other) {}
    Entity entity;

private:
    std::string filepath;
};
