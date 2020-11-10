#pragma once

#include "Core/Layer.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"

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
    static SharedPtr<VertexArray> vao;
    static SharedPtr<Shader> shader;
};
