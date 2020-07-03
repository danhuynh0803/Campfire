#ifndef RENDER_LAYER_H
#define RENDER_LAYER_H

#include "Core/Layer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"

class RenderLayer : public Layer
{
public:
    RenderLayer();
    ~RenderLayer() = default;

    void RenderLayer::DrawTriangles();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate() override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& event) override;

    Shader* shader;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<VertexArray> vertexArray;
    uint32_t VAO;
};

#endif // RENDER_LAYER_H
