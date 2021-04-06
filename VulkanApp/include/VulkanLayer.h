#pragma once

#include "Renderer/Buffer.h"
#include "Core/Layer.h"
#include "Core/Timer.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanBuffer.h"
#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanMesh.h"
#include "Vulkan/VulkanPipeline.h"

class Scene;

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

    bool OnWindowResize(WindowResizeEvent& e);

private:
    void SceneHierarchy(SharedPtr<Scene> scene);

    SharedPtr<vk::VulkanMesh> meshPtr;
    SharedPtr<VulkanVertexBuffer> vertexBufferPtr;
    SharedPtr<VulkanBuffer> pVertexBuffer;
    SharedPtr<VulkanIndexBuffer> indexBufferPtr;
    std::vector<SharedPtr<VulkanUniformBuffer>> cameraUBOs;
    std::vector<SharedPtr<VulkanUniformBuffer>> transformUBOs;
    std::vector<SharedPtr<VulkanUniformBuffer>> lightUBOs;
    std::vector<SharedPtr<VulkanTexture2D>> textures;
    Timer timer;

    GraphicsPipeline::TransformPushConstBlock mPushConstBlock;

    SharedPtr<Scene> scene;
};