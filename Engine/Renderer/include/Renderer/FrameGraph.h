#pragma once

#include <vulkan/vulkan.hpp>
#include <map>

class FrameGraph
{
public:
    void CreateRenderFrameGraph();
    vk::Device mDevice;
    vk::UniqueRenderPass mRenderPass;

private:

    // TODO might be useful to use labels with subpasses for debugging
    //template <typename T>
    //using LabelMap = std::map<std::string, T>;
    //LabelMap<vk::SubpassDescription> mSubpasses;
    //LabelMap<vk::SubpassDependency> mSubpassDependencies;
    //LabelMap<vk::RenderPass> mRenderpasses;
};
