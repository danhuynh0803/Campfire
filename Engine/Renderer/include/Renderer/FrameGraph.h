#pragma once

#include <vulkan/vulkan.hpp>
#include <map>

class FrameGraph
{
public:
    void CreateRenderFrameGraph();
    vk::Device mDevice;

    vk::RenderPass GetRenderPass(const std::string& label) { return mRenderPasses.at(label).get(); }
private:
    void CreateOpaque();

    template <typename T>
    using LabelMap = std::map<std::string, T>;
    LabelMap<vk::UniqueRenderPass> mRenderPasses;
    LabelMap<vk::DescriptorSetLayout> mDescriptorSetLayouts;
    LabelMap<vk::DescriptorSet> mDescriptorSets;
    LabelMap<vk::PipelineLayout> mPipelineLayouts;
    LabelMap<vk::Pipeline> mPipelines;
    //LabelMap<vk::SubpassDescription> mSubpasses;
    //LabelMap<vk::SubpassDependency> mSubpassDependencies;
};
