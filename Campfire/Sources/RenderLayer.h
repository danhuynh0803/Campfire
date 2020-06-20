#ifndef RENDER_LAYER_H
#define RENDER_LAYER_H

#include "Core/Layer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class RenderLayer : public Layer
{
public:
    RenderLayer();
    ~RenderLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate() override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& event) override;

    GLuint VAO;
};

#endif // RENDER_LAYER_H
