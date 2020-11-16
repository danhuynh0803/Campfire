#include "Scene/ScriptableEntity.h"

template <typename T>
T& ScriptableEntity::GetComponent()
{
    return entity.GetComponent<T>();
}

Entity ScriptableEntity::Instantiate(Entity newEntity, glm::vec3 position)
{
    // TODO once duplication is added
    //Entity newEntity = entity.scene->CreateEntity("InstObject");
    //newEntity.GetComponent<TransformComponent>().position = position;
    //newEntity.AddComponent<MeshComponent>(MeshComponent::Geometry::CUBE);

    return newEntity;
}

void ScriptableEntity::Destroy(Entity other, float timer)
{
    entity.scene->RemoveEntity(other);
}

void ScriptableEntity::Destroy(ScriptableEntity* other, float timer)
{
    entity.scene->RemoveEntity(other->entity);
}
