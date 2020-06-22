#ifndef RENDER_LAYER_H
#define RENDER_LAYER_H

#include "Core/Layer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"

#include "Renderer/Buffer.h"

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

    GLuint VAO;
    Shader shader;
    std::unique_ptr<IndexBuffer> indexBuffer;
};

#endif // RENDER_LAYER_H
