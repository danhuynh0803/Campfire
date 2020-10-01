#pragma once

#include <vulkan/vulkan.hpp>

class VulkanSwapChain
{
public:
    VulkanSwapChain();
    ~VulkanSwapChain() = default;
    void Present();

    uint32_t mImageIndex = 0;
    uint32_t GetCurrentImageIndex() { return mImageIndex; }
    size_t currentFrame = 0;

    int maxFramesInFlight = 2;

    void CreateFramebuffers();
    void CreateBarriers();
    void DrawIndexed(vk::Buffer vertexBuffer, vk::Buffer indexBuffer, uint32_t count);
    vk::UniqueCommandPool CreateCommandPool(uint32_t queueFamilyIndex);
    std::vector<vk::UniqueCommandBuffer> CreateCommandBuffers(uint32_t size);

    vk::CommandBuffer GetCurrentCommandBuffer() { return commandBuffers.at(mImageIndex).get(); }
    vk::Framebuffer GetCurrentFramebuffer() { return swapChainFramebuffers.at(mImageIndex).get(); }

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

    // Semaphores
    std::vector<vk::UniqueSemaphore> imageAvailableSemaphores;
    std::vector<vk::UniqueSemaphore> renderFinishedSemaphores;
    std::vector<vk::UniqueFence> inFlightFences;
    std::vector<vk::Fence> imagesInFlight;
};
