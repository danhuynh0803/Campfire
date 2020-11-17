#pragma once

#include "Scene/ScriptableEntity.h"
#include "Core/Input.h"

namespace NativeScript
{
    class CameraController : public ScriptableEntity
    {
    public:
        void Start()
        {
        }

        void Update(float dt)
        {
            auto& transform = GetComponent<TransformComponent>();
            float speed = 5.0f;

            //if (Input::GetKey(KEY_LEFT))
            if (Input::GetButton("KEY_LEFT"))
                transform.position.x -= speed * dt;
            //else if (Input::GetKey(KEY_RIGHT))
            else if (Input::GetButton("KEY_RIGHT"))
                transform.position.x += speed * dt;

            //if (Input::GetKey(KEY_UP))
            if (Input::GetButton("KEY_UP"))
                transform.position.y += speed * dt;
            //else if (Input::GetKey(KEY_DOWN))
            else if (Input::GetButton("KEY_DOWN"))
                transform.position.y -= speed * dt;
        }
    };
}
