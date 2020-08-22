#include "Scene/Entity.h"
#include "Scene/ScriptableEntity.h"
#include "Core/Input.h"

namespace Script
{
    class CameraController : public ScriptableEntity
    {
    public:
        // TODO have this affect runtime transform instead
        void Start()
        {
            LOG_INFO("Camera Controller start");
            auto& transform = GetComponent<TransformComponent>();
            transform.position = glm::vec3(0.0f, 0.0f, 10.0f);
        }

        void Destroy()
        {

        }

        void Update(float dt)
        {
            auto& transform = GetComponent<TransformComponent>();
            float speed = 5.0f;

            if (Input::GetKey(KEY_A))
                transform.position.x -= speed * dt;
            else if (Input::GetKey(KEY_D))
                transform.position.x += speed * dt;

            if (Input::GetKey(KEY_W))
                transform.position.y += speed * dt;
            else if (Input::GetKey(KEY_S))
                transform.position.y -= speed * dt;
        }
    };
}
