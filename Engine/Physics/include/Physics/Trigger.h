#pragma once

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <glm/glm.hpp>
#include <entt.hpp>

class Trigger
{
public:
    void Construct(const glm::vec3& pos, const glm::vec3& euler, const glm::vec3& scale);

    btGhostObject* GetBulletGhostObject() { return ghostObject; }

    std::vector<entt::entity> prevList;
    std::vector<entt::entity> overlapEnterList;
    std::vector<entt::entity> overlapExitList;
    std::vector<entt::entity> overlapStayList;

private:

    btGhostObject* ghostObject = nullptr;
};
