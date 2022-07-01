#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>
//#include "Renderer/FrameGraph.h"
#include "Renderer/GraphicsContext.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanSwapChain.h"
//#include "Vulkan/VulkanGraphicsPipeline.h"
//#include "Vulkan/VulkanComputePipeline.h"


class GLFWwindow;
class FrameGraph;

class VulkanContext : public GraphicsContext
{
public:
    VulkanContext(GLFWwindow* windowHandle);
    ~VulkanContext();

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
    SharedPtr<VulkanSwapChain> GetSwapChain() { return mSwapChain; }

    SharedPtr<FrameGraph> mFrameGraph;

private:
    vk::UniqueInstance CreateInstance();
private:
    inline static vk::UniqueInstance sVulkanInstance;
    inline static SharedPtr<VulkanContext> sVulkanContextInstance;
    SharedPtr<VulkanDevice> mDevice;
    vk::UniqueCommandPool mGraphicsCommandPool;
    vk::UniqueCommandPool mComputeCommandPool;
    vk::UniqueDescriptorPool mDescriptorPool;
    SharedPtr<VulkanSwapChain> mSwapChain;

    GLFWwindow* windowHandle;
    bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);
};
