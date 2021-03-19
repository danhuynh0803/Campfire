#pragma once

#include "Core/Layer.h"
#include "Scene/Camera.h"

class Scene;

class GameLayer : public Layer
{
public:
    GameLayer();
    ~GameLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& event) override;

private:
    SharedPtr<Scene> activeScene = nullptr;
    SharedPtr<Camera> mainGameCamera = nullptr;
};