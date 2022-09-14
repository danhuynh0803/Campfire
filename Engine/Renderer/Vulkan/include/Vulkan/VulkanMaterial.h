#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

class VulkanUniformBuffer;

struct VulkanMaterial
{
    glm::vec4 baseColor = glm::vec4(1.0f);
    uint32_t albedoMapIndex;
    uint32_t normalMapIndex;
    uint32_t metallicMapIndex;
    uint32_t roughnessMapIndex;
    uint32_t occlusionMapIndex;
    uint32_t emissiveMapIndex;

    bool useAlbedoMap    = false;
    bool useMetallicMap  = false;
    bool useNormalMap    = false;
    bool useRoughnessMap = false;
    bool useOcclusionMap = false;
    bool useEmissiveMap  = false;

    SharedPtr<Texture2D> albedoMap    = nullptr;
    SharedPtr<Texture2D> metallicMap  = nullptr;
    SharedPtr<Texture2D> normalMap    = nullptr;
    SharedPtr<Texture2D> roughnessMap = nullptr;
    SharedPtr<Texture2D> occlusionMap = nullptr;
    SharedPtr<Texture2D> emissiveMap  = nullptr;

    SharedPtr<VulkanUniformBuffer> textureMapUsageUbo = nullptr;

    std::vector<vk::UniqueDescriptorSet> mDescriptorSets;
    std::vector<vk::UniqueDescriptorSetLayout> mDescriptorSetLayouts;
    vk::Pipeline mPipeline;
};
