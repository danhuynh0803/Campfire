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
    vk::Format GetFormat() { return swapChainImageFormat; }
    std::vector<vk::Image> GetImages() { return swapChainImages; }
    vk::Extent2D GetExtent() { return swapChainExtent; }

    void CreateFramebuffers();
    void CreateBarriers();

    uint32_t GetWidth() { return mWidth; }
    uint32_t GetHeight() { return mHeight; }

private:
    vk::UniqueSurfaceKHR CreateSurfaceKHR(GLFWwindow* window);
    vk::UniqueCommandPool CreateCommandPool(uint32_t queueFamilyIndex);
    std::vector<vk::UniqueCommandBuffer> CreateCommandBuffers(uint32_t size);

    uint32_t mImageIndex = 0;
    size_t currentFrame = 0;
    int maxFramesInFlight = 2;

    uint32_t mWidth;
    uint32_t mHeight;

    // SwapChain - related
    vk::Format swapChainImageFormat;
    vk::UniqueSurfaceKHR mSurface;
    vk::UniqueSwapchainKHR swapChain;
    vk::Extent2D swapChainExtent;

    // Command buffers
    vk::UniqueCommandPool commandPool;
    std::vector<vk::UniqueCommandBuffer> commandBuffers;

    // Framebuffer
    std::vector<vk::UniqueFramebuffer> swapChainFramebuffers;
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::UniqueImageView> imageViews;

    // Barries
    std::vector<vk::UniqueSemaphore> imageAvailableSemaphores;
    std::vector<vk::UniqueSemaphore> renderFinishedSemaphores;
    std::vector<vk::UniqueFence> inFlightFences;
    std::vector<vk::Fence> imagesInFlight;
};