#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

struct VulkanMaterial
{
    glm::vec4 baseColor = glm::vec4(1.0f);
    uint32_t albedoMapIndex;
    uint32_t normalMapIndex;
    //uint32_t metallicMapIndex;
    //uint32_t roughnessMapIndex;
    //uint32_t occlusionMapIndex;
    //uint32_t emissiveMapIndex;

    bool useAlbedoMap    = false;
    bool useMetallicMap  = false;
    bool useNormalMap    = false;
    bool useRoughnessMap = false;
    bool useOcclusionMap = false;
    bool useEmissiveMap = false;

    SharedPtr<Texture2D> albedoMap = nullptr;
    SharedPtr<Texture2D> normalMap = nullptr;

    std::vector<vk::UniqueDescriptorSet> descriptorSets;
    //vk::Pipeline pipeline;
};
