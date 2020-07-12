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

    void RenderLayer::DrawTriangles();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float timestep) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& event) override;

    // TODO just for testing
    SharedPtr<UniformBuffer> ubo;
    SharedPtr<IndexBuffer> indexBuffer;
    SharedPtr<VertexArray> vertexArray;

    SharedPtr<Shader> shader;
    SharedPtr<Texture> texture;
};

#endif // RENDER_LAYER_H
