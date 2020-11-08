#pragma once

#include "Renderer/RendererAPI.h"
#include <vulkan/vulkan.hpp>

class VulkanRendererAPI : public RendererAPI
{
    virtual void Init() override;
    virtual void SetClearColor(const glm::vec4& color) override;
    virtual void Clear() override;

    virtual void DrawIndexed(const SharedPtr<VertexArray>& vertexArray) override;
    virtual void DrawIndexedLines(const SharedPtr<VertexArray>& vertexArray) override;

    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
};
