#pragma once

#include "Scene/ScriptableEntity.h"
#include "Core/Input.h"
#include "Scene/Component.h"

class PlayerController : public ScriptableEntity
{
public:
    float speed = 5.0f;
    float timer = 1.0f;
    float jumpForce = 5.0f;
    SharedPtr<Rigidbody> rb;
    SharedPtr<AudioSource> audioSource;

    // TODO put this somewhere all scripts have access to
    // Add force to rb
    glm::vec3 right = glm::vec3(1, 0, 0);
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 forward = glm::vec3(0, 0, 1);

public:
    void Start()
    {
        rb = GetComponent<RigidbodyComponent>();
        audioSource = GetComponent<AudioComponent>();
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
        LOG_INFO("Collider Enter {0}", other.GetComponent<TagComponent>().tag);
        SharedPtr<Rigidbody> rb = other.GetComponent<RigidbodyComponent>();
        //rb->AddVelocity(up * jumpForce * 5.0f);

        audioSource->Play();

        auto material = other.GetComponent<MeshComponent>().material;
        // FIXME material color change is being
        // carried over to editor scene
        // Check scene copy, maybe scenes are sharing
        // the same material instead of creating a copy
        material->color = glm::vec4(0, 0, 1, 1);

        // FIXME crashing when restarting scene
        // maybe has to do with referencing new object
        // that no longer exists when scene stops
        Instantiate(other, glm::vec3(0, 10, 0));
    }

    void OnTriggerStay(Entity other)
    {
        //LOG_INFO("Collider Stay {0}", other.GetComponent<TagComponent>().tag);
        //SharedPtr<Rigidbody> rb = other.GetComponent<RigidbodyComponent>();
        //rb->AddVelocity(up * jumpForce);
    }

    void OnTriggerExit(Entity other)
    {
        LOG_INFO("Collider Exit {0}", other.GetComponent<TagComponent>().tag);
        //SharedPtr<Rigidbody> rb = other.GetComponent<RigidbodyComponent>();
        //rb->AddVelocity(up * jumpForce);

        auto material = other.GetComponent<MeshComponent>().material;
        material->color = glm::vec4(1, 0, 0, 1);
    }
};
