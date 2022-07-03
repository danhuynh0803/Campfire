#pragma once

#include <vulkan/vulkan.hpp>

struct GLFWwindow;

class VulkanSwapChain
{
public:
    VulkanSwapChain(GLFWwindow* window);
    ~VulkanSwapChain() = default;
    void Present();

    uint32_t GetCurrentImageIndex() { return mImageIndex; }
    vk::CommandBuffer GetCurrentCommandBuffer() { return commandBuffers.at(mImageIndex).get(); }
    vk::CommandBuffer GetCommandBuffer(uint32_t index) { return commandBuffers.at(index).get(); }

    vk::Framebuffer GetCurrentFramebuffer() { return swapChainFramebuffers.at(mImageIndex).get(); }
    vk::Framebuffer GetFramebuffer(uint32_t index) { return swapChainFramebuffers.at(index).get(); }
    vk::SurfaceKHR GetSurface() { return mSurface.get(); }
    vk::Format GetFormat() { return swapChainImageFormat; }
    std::vector<vk::Image> GetImages() { return swapChainImages; }
    vk::Extent2D GetExtent() { return swapChainExtent; }

    void CreateFramebuffers(const vk::RenderPass& renderPass);
    void CreateBarriers();

    uint32_t GetWidth() { return mWidth; }
    uint32_t GetHeight() { return mHeight; }

private:
    vk::UniqueSurfaceKHR CreateSurfaceKHR(GLFWwindow* window);

    uint32_t mImageIndex = 0;
    size_t mCurrentFrame = 0;
    int mMaxFramesInFlight = 2;

    uint32_t mWidth;
    uint32_t mHeight;

    // SwapChain - related
    vk::Format swapChainImageFormat;
    vk::UniqueSurfaceKHR mSurface;
    vk::UniqueSwapchainKHR swapChain;
    vk::Extent2D swapChainExtent;

    // Command buffers
    std::vector<vk::UniqueCommandBuffer> commandBuffers;

    // Framebuffer
    std::vector<vk::UniqueFramebuffer> swapChainFramebuffers;
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::UniqueImageView> imageViews;

    // Barriers
    std::vector<vk::UniqueSemaphore> imageAvailableSemaphores;
    std::vector<vk::UniqueSemaphore> renderFinishedSemaphores;
    std::vector<vk::UniqueFence> inFlightFences;
    std::vector<vk::Fence> imagesInFlight;

    // Depth buffer related
    vk::UniqueImage depthImage;
    vk::UniqueDeviceMemory depthImageMemory;
    vk::UniqueImageView depthImageView;
};
