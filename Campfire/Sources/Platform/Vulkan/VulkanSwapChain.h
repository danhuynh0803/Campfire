#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

class VulkanSwapChain
{
public:
    VulkanSwapChain(GLFWwindow* window);
    ~VulkanSwapChain() = default;
    void Present();

    uint32_t GetCurrentImageIndex() { return mImageIndex; }
    vk::CommandBuffer GetCurrentCommandBuffer() { return commandBuffers.at(mImageIndex).get(); }
    vk::Framebuffer GetCurrentFramebuffer() { return swapChainFramebuffers.at(mImageIndex).get(); }
    vk::SurfaceKHR GetSurface() { return mSurface.get(); }
    vk::CommandPool GetCommandPool() { return commandPool.get(); }
    void CreateFramebuffers();
    void CreateBarriers();

    uint32_t GetWidth() { return mWidth; }
    uint32_t GetHeight() { return mHeight; }

private:
    vk::UniqueSurfaceKHR CreateSurfaceKHR(GLFWwindow* window);
    vk::UniqueCommandPool CreateCommandPool(uint32_t queueFamilyIndex);
    std::vector<vk::UniqueCommandBuffer> CreateCommandBuffers(uint32_t size);

private:
    uint32_t mImageIndex = 0;
    size_t currentFrame = 0;
    int maxFramesInFlight = 2;

    uint32_t mWidth;
    uint32_t mHeight;

public:
    // SwapChain
    vk::Format swapChainImageFormat;
    vk::UniqueSurfaceKHR mSurface;
    vk::UniqueSwapchainKHR swapChain;
    vk::Extent2D swapChainExtent;
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::UniqueImageView> imageViews;

    // Command buffers
    vk::UniqueCommandPool commandPool;
    std::vector<vk::UniqueCommandBuffer> commandBuffers;

    // Framebuffer
    std::vector<vk::UniqueFramebuffer> swapChainFramebuffers;

    // Semaphores
    std::vector<vk::UniqueSemaphore> imageAvailableSemaphores;
    std::vector<vk::UniqueSemaphore> renderFinishedSemaphores;
    std::vector<vk::UniqueFence> inFlightFences;
    std::vector<vk::Fence> imagesInFlight;
};
