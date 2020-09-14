#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "Renderer/GraphicsContext.h"

#include <vector>

class VulkanContext : public GraphicsContext
{
public:
    VulkanContext(GLFWwindow* windowHandle);
    ~VulkanContext();

    virtual void Init() override;
    virtual void SwapBuffers() override;

private:
    vk::UniqueInstance CreateInstance();
    vk::PhysicalDevice GetPhysicalDevice();
    bool IsDeviceSuitable(vk::PhysicalDevice device);
    bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);
    void CreateGraphicsPipeline();
    void CreateFramebuffers();
    void CreateCommandPool();

private:
    vk::UniqueInstance instance;

    std::vector<vk::PhysicalDevice> physicalDevices;
    vk::PhysicalDevice physicalDevice;

    vk::Format swapChainImageFormat;
    vk::UniqueSurfaceKHR surface;
    vk::UniqueSwapchainKHR swapChain;
    vk::Extent2D swapChainExtent;
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::UniqueImageView> imageViews;

    std::vector<vk::QueueFamilyProperties> queueFamilyProperties;

    vk::UniqueDevice device;

    vk::UniqueCommandPool commandPool;
    std::vector<vk::UniqueCommandBuffer> commandBuffers;

    vk::UniquePipelineLayout pipelineLayout;
    vk::UniqueRenderPass renderPass;
    vk::UniquePipeline graphicsPipeline;

    std::vector<vk::UniqueFramebuffer> swapChainFramebuffers;

    GLFWwindow* windowHandle;
};
