#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "Renderer/GraphicsContext.h"

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
    vk::PhysicalDevice GetPhysicalDevice() { return physicalDevice; }
    vk::Device GetDevice() { return device.get(); }
    // TODO update to take some enum and can get various queues
    vk::Queue GetQueue() { return graphicsQueue; }
    //std::vector<vk::UniqueDescriptorSet>& GetDescriptorSets { return descriptorSets; }

    std::vector<vk::UniqueDescriptorSet> descriptorSets;

    void DrawIndexed(vk::Buffer vertexBuffer, vk::Buffer indexBuffer, uint32_t count);

private:
    // Instance
    vk::UniqueInstance CreateInstance();
    bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

    // PhysicalDevice
    vk::PhysicalDevice SelectPhysicalDevice();
    bool IsDeviceSuitable(vk::PhysicalDevice device);

    // SwapChain
    vk::UniqueSurfaceKHR CreateSurfaceKHR(GLFWwindow* window);
    void SetupSwapChain();

    // Logical device
    vk::UniqueDevice CreateLogicalDevice();

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
    vk::UniqueInstance instance;

    // PhysicalDevice
    std::vector<vk::PhysicalDevice> physicalDevices;
    // The selected physical device that will run vulkan
    vk::PhysicalDevice physicalDevice;
    // Logical device
    vk::UniqueDevice device;

    // SwapChain
    vk::Format swapChainImageFormat;
    vk::UniqueSurfaceKHR surface;
    vk::UniqueSwapchainKHR swapChain;
    vk::Extent2D swapChainExtent;
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::UniqueImageView> imageViews;
    size_t currentFrame = 0;

    // Queues
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties;

    // Graphics pipeline
    vk::UniqueDescriptorPool descriptorPool;

    vk::UniqueDescriptorSetLayout descriptorSetLayout;
    vk::UniquePipelineLayout pipelineLayout;
    vk::UniqueRenderPass renderPass;
    vk::UniquePipeline graphicsPipeline;

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
