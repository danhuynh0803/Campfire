#pragma once

#include <vulkan/vulkan.hpp>

class VulkanSwapChain
{
public:
    VulkanSwapChain();
    ~VulkanSwapChain();

private:
    // Command buffers
    vk::UniqueCommandPool commandPool;
    std::vector<vk::UniqueCommandBuffer> commandBuffers;

    // Framebuffer
    std::vector<vk::UniqueFramebuffer> swapChainFramebuffers;

    // SwapChain
    vk::Format swapChainImageFormat;
    vk::UniqueSurfaceKHR surface;
    vk::UniqueSwapchainKHR swapChain;
    vk::Extent2D swapChainExtent;
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::UniqueImageView> imageViews;
    size_t currentFrame = 0;

    // Semaphores
    std::vector<vk::UniqueSemaphore> imageAvailableSemaphores;
    std::vector<vk::UniqueSemaphore> renderFinishedSemaphores;
    std::vector<vk::UniqueFence> inFlightFences;
    std::vector<vk::Fence> imagesInFlight;
};
