#pragma once

#include "Core/Layer.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanBuffer.h"
#include "Renderer/Buffer.h"

class VulkanLayer : public Layer
{
public:
    VulkanLayer();
    ~VulkanLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& event) override;

private:
    SharedPtr<VulkanVertexBuffer> vertexBufferPtr;
    SharedPtr<VulkanIndexBuffer> indexBufferPtr;
    std::vector<SharedPtr<VulkanUniformBuffer>> uniformBufferPtrs;
};
