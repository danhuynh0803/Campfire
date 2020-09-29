#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "Renderer/GraphicsContext.h"
#include "VulkanDevice.h"

class VulkanContext : public GraphicsContext
{
public:
    static SharedPtr<VulkanContext> GetInstance() { return contextInstance; }

    static SharedPtr<VulkanContext> contextInstance;

    VulkanContext(GLFWwindow* windowHandle);
    ~VulkanContext();

    virtual void Init() override;
    virtual void SwapBuffers() override;

    vk::CommandPool GetCommandPool() { return commandPool.get(); }

    std::vector<vk::UniqueDescriptorSet> descriptorSets;

    void DrawIndexed(vk::Buffer vertexBuffer, vk::Buffer indexBuffer, uint32_t count);

    static vk::Instance GetVulkanInstance() { return instance.get(); }

    static vk::SurfaceKHR GetSurface() { return surface.get(); }

    SharedPtr<VulkanDevice> GetVulkanDevice() { return devicePtr; }

private:
    vk::UniqueInstance CreateInstance();

    bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

    // SwapChain
    vk::UniqueSurfaceKHR CreateSurfaceKHR(GLFWwindow* window);
    void SetupSwapChain();

    // Graphics pipeline
    vk::UniquePipeline CreateGraphicsPipeline();

    // Framebuffers
    void CreateFramebuffers();

    // Command pool/buffer
    vk::UniqueCommandPool CreateCommandPool(uint32_t queueFamilyIndex);
    std::vector<vk::UniqueCommandBuffer> CreateCommandBuffers(uint32_t size);

    // Semaphores
    vk::UniqueSemaphore CreateSemaphore();

    vk::UniqueFence CreateFence();

private:
    // Instance
    static vk::UniqueInstance instance;

    SharedPtr<VulkanDevice> devicePtr;

    // Graphics pipeline
    vk::UniqueDescriptorPool descriptorPool;

    vk::UniqueDescriptorSetLayout descriptorSetLayout;
    vk::UniquePipelineLayout pipelineLayout;
    vk::UniqueRenderPass renderPass;
    vk::UniquePipeline graphicsPipeline;

    // SwapChain
    vk::Format swapChainImageFormat;
    static vk::UniqueSurfaceKHR surface;
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

    GLFWwindow* windowHandle;
};
