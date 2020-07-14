#ifndef RENDER_LAYER_H
#define RENDER_LAYER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/Base.h"
#include "Core/Layer.h"
#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Texture.h"

class RenderLayer : public Layer
{
public:
    RenderLayer();
    ~RenderLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& event) override;

private:
    SharedPtr<UniformBuffer> ubo;
    SharedPtr<Texture2D> texture2D;

    SharedPtr<FrameBuffer> colorFB;
};

#endif // RENDER_LAYER_H
