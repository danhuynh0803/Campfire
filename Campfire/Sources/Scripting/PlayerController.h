#include "Scene/ScriptableEntity.h"
#include "Core/Input.h"

namespace Script
{
    class PlayerController : public ScriptableEntity
    {
    public:
        float speed = 5.0f;
        float timer = 1.0f;

    public:
        // TODO have this affect runtime transform instead
        void Start()
        {
        }

        void Update(float dt)
        {
            if ((timer -= dt) <= 0.0f)
            {
                //Destroy(this);
                //LOG_INFO("Object destroyed");
                //return;
            }

            auto& transform = GetComponent<TransformComponent>();

            if (Input::GetKey(KEY_A))
                transform.position.x -= speed * dt;
            else if (Input::GetKey(KEY_D))
                transform.position.x += speed * dt;
            if (Input::GetKey(KEY_W))
                transform.position.y += speed * dt;
            else if (Input::GetKey(KEY_S))
                transform.position.y -= speed * dt;

            // Jump
            if (Input::GetKeyDown(KEY_SPACE))
            {
                // Add force to rb
            }
        }
    };
}
