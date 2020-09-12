#pragma once

#include <vulkan/vulkan.hpp>

class VulkanContext
{
public:
    VulkanContext();
    ~VulkanContext();

private:
    vk::Instance CreateInstance();

private:
    vk::Instance instance;
};
