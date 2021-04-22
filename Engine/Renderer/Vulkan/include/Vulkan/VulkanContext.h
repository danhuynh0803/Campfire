#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>
#include "Renderer/Renderer.h"
#include "Renderer/FrameGraph.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanGraphicsPipeline.h"
#include "Vulkan/VulkanComputePipeline.h"
#include "Vulkan/VulkanSwapChain.h"

class GLFWwindow;

class VulkanContext : public GraphicsContext
{
public:
    VulkanContext(GLFWwindow* windowHandle);
    ~VulkanContext() {}

    virtual void Init() override;
    virtual void SwapBuffers() override;
    void CleanSwapChain();
    void RecreateSwapChain();

    // TODO use this one and remove static var,
    // but only after graphics pipeline is refactored out of context
    //static SharedPtr<VulkanContext> Get() { return std::dynamic_pointer_cast<VulkanContext>(Renderer::GetContext()); }
    static SharedPtr<VulkanContext> Get() { return sVulkanContextInstance; }
    static vk::Instance GetInstance() { return sVulkanInstance.get(); }
    //static SharedPtr<VulkanDevice> GetContextDevice() { return Get()->mDevice(); }
    SharedPtr<VulkanDevice> GetDevice() { return mDevice; }
    vk::CommandPool GetCommandPool(QueueFamilyType type);
    vk::UniqueCommandPool CreateCommandPool(uint32_t queueFamilyIndex);
    std::vector<vk::UniqueCommandBuffer> CreateCommandBuffers(uint32_t size);
    vk::DescriptorPool GetDescriptorPool() { return mDescriptorPool.get(); }

    SharedPtr<VulkanGraphicsPipeline> GetGraphicsPipeline() { return mGraphicsPipeline; }
    SharedPtr<VulkanSwapChain> GetSwapChain() { return mSwapChain; }

    FrameGraph mFrameGraph;
    SharedPtr<VulkanComputePipeline> mComputePipeline;

private:
    vk::UniqueDescriptorPool mDescriptorPool;
    vk::UniqueCommandPool mGraphicsCommandPool;
    vk::UniqueCommandPool mComputeCommandPool;
    vk::UniqueInstance CreateInstance();
    inline static vk::UniqueInstance sVulkanInstance;
    inline static SharedPtr<VulkanContext> sVulkanContextInstance;
    GLFWwindow* windowHandle;
    bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

    SharedPtr<VulkanDevice> mDevice;
    SharedPtr<VulkanGraphicsPipeline> mGraphicsPipeline;
    SharedPtr<VulkanSwapChain> mSwapChain;
};
