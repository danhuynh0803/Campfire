#include "Scene/ScriptableEntity.h"
#include "Core/Input.h"

namespace Script
{
    class PlayerController : public ScriptableEntity
    {
    public:
        float speed = 5.0f;
        float timer = 1.0f;
        float jumpForce = 5.0f;
        SharedPtr<Rigidbody> rb;

        // TODO put this somewhere all scripts have access to
        // Add force to rb
        glm::vec3 right = glm::vec3(1, 0, 0);
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 forward = glm::vec3(0, 0, 1);

    public:
        // TODO have this affect runtime transform instead
        void Start()
        {
            rb = GetComponent<RigidbodyComponent>();
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

            float nSpeed = speed * dt;
            if (Input::GetKey(KEY_A))
                rb->AddVelocity(-right * nSpeed);
            else if (Input::GetKey(KEY_D))
                rb->AddVelocity(right * nSpeed);
            if (Input::GetKey(KEY_W))
                rb->AddVelocity(-forward * nSpeed);
            else if (Input::GetKey(KEY_S))
                rb->AddVelocity(forward * nSpeed);

            // Jump
            if (Input::GetKeyDown(KEY_SPACE))
            {
                rb->AddVelocity(up * jumpForce);
            }
        }

        void OnTriggerEnter(Entity other)
        {
            // TODO have it process just once
            // This is more of a OnTriggerStay
            LOG_INFO("Collider entity {0}", other.GetComponent<TagComponent>().tag);

            SharedPtr<Rigidbody> rb = other.GetComponent<RigidbodyComponent>();
            rb->AddVelocity(up * jumpForce);
        }
    };
}
