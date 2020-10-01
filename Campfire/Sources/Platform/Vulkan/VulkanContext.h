#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "Renderer/Renderer.h"
#include "VulkanDevice.h"
#include "VulkanPipeline.h"

class VulkanContext : public GraphicsContext
{
public:
    VulkanContext(GLFWwindow* windowHandle);
    ~VulkanContext();

    virtual void Init() override;
    virtual void SwapBuffers() override;

    // TODO use this one and remove static var,
    // but only after graphics pipeline is refactored out of context
    //static SharedPtr<VulkanContext> Get() { return std::dynamic_pointer_cast<VulkanContext>(Renderer::GetContext()); }
    static SharedPtr<VulkanContext> Get() { return sVulkanContextInstance; }
    static vk::Instance GetInstance() { return sVulkanInstance.get(); }
    static vk::SurfaceKHR GetSurface() { return surface.get(); }
    SharedPtr<VulkanDevice> GetDevice() { return mDevice; }

private:
    vk::UniqueInstance CreateInstance();
    inline static vk::UniqueInstance sVulkanInstance;
    inline static SharedPtr<VulkanContext> sVulkanContextInstance;
    SharedPtr<VulkanDevice> mDevice;
    GLFWwindow* windowHandle;

    bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

// ============ TODO move below to other files ================
public:
    vk::CommandPool GetCommandPool() { return commandPool.get(); }

    SharedPtr<VulkanPipeline> mGraphicsPipeline;

    // move to renderer
    void DrawIndexed(vk::Buffer vertexBuffer, vk::Buffer indexBuffer, uint32_t count);

    // SwapChain
    vk::UniqueSurfaceKHR CreateSurfaceKHR(GLFWwindow* window);
    void SetupSwapChain();

    // Framebuffers
    void CreateFramebuffers();

    // Command pool/buffer
    vk::UniqueCommandPool CreateCommandPool(uint32_t queueFamilyIndex);
    std::vector<vk::UniqueCommandBuffer> CreateCommandBuffers(uint32_t size);

    // Semaphores
    vk::UniqueSemaphore CreateSemaphore();

    vk::UniqueFence CreateFence();

    // SwapChain
    vk::Format swapChainImageFormat;
    inline static vk::UniqueSurfaceKHR surface;
    vk::UniqueSwapchainKHR swapChain;
    vk::Extent2D swapChainExtent;
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::UniqueImageView> imageViews;
    size_t currentFrame = 0;

    // Framebuffers
    std::vector<vk::UniqueFramebuffer> swapChainFramebuffers;

    // Command pool/buffer
    vk::UniqueCommandPool commandPool;
    std::vector<vk::UniqueCommandBuffer> commandBuffers;

    // Semaphores
    std::vector<vk::UniqueSemaphore> imageAvailableSemaphores;
    std::vector<vk::UniqueSemaphore> renderFinishedSemaphores;
    std::vector<vk::UniqueFence> inFlightFences;
    std::vector<vk::Fence> imagesInFlight;
};
