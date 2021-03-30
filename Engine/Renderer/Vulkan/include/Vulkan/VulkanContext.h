#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>
#include "Renderer/Renderer.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanPipeline.h"
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
    vk::CommandPool GetCommandPool() { return commandPool.get(); }
    vk::UniqueCommandPool CreateCommandPool(uint32_t queueFamilyIndex);
    std::vector<vk::UniqueCommandBuffer> CreateCommandBuffers(uint32_t size);

    SharedPtr<VulkanPipeline> GetPipeline() { return mGraphicsPipeline; }
    SharedPtr<VulkanSwapChain> GetSwapChain() { return mSwapChain; }

private:
    vk::UniqueCommandPool commandPool;
    vk::UniqueInstance CreateInstance();
    inline static vk::UniqueInstance sVulkanInstance;
    inline static SharedPtr<VulkanContext> sVulkanContextInstance;
    GLFWwindow* windowHandle;
    bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

    SharedPtr<VulkanDevice> mDevice;
    SharedPtr<VulkanPipeline> mGraphicsPipeline;
    SharedPtr<VulkanSwapChain> mSwapChain;
};
