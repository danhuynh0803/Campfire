#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "Renderer/Renderer.h"
#include "VulkanDevice.h"
#include "VulkanPipeline.h"
#include "VulkanSwapChain.h"

class VulkanContext : public GraphicsContext
{
public:
    VulkanContext(GLFWwindow* windowHandle);
    ~VulkanContext() {}

    virtual void Init() override;
    virtual void SwapBuffers() override;

    // TODO use this one and remove static var,
    // but only after graphics pipeline is refactored out of context
    //static SharedPtr<VulkanContext> Get() { return std::dynamic_pointer_cast<VulkanContext>(Renderer::GetContext()); }
    static SharedPtr<VulkanContext> Get() { return sVulkanContextInstance; }
    static vk::Instance GetInstance() { return sVulkanInstance.get(); }
    static vk::SurfaceKHR GetSurface() { return mSurface.get(); }
    SharedPtr<VulkanDevice> GetDevice() { return mDevice; }

private:
    vk::UniqueInstance CreateInstance();
    inline static vk::UniqueInstance sVulkanInstance;
    inline static SharedPtr<VulkanContext> sVulkanContextInstance;
    inline static vk::UniqueSurfaceKHR mSurface;
    SharedPtr<VulkanDevice> mDevice;
    GLFWwindow* windowHandle;

    bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

// ============ TODO move below to other files ================
public:
    //vk::CommandPool GetCommandPool() { return commandPool.get(); }

    // Maybe move to swap chain?
    vk::UniqueSurfaceKHR CreateSurfaceKHR(GLFWwindow* window);
    SharedPtr<VulkanPipeline> mGraphicsPipeline;
    SharedPtr<VulkanSwapChain> mSwapChain;
};
