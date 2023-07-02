#include "Core/Base.h"
#include "Core/ResourceManager.h"

#include "Renderer/FrameGraph.h"

#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanInitializers.h"
#include "Vulkan/VulkanShader.h"
#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanSwapChain.h"

#include <vector>

FrameGraph::FrameGraph()
{
}

void FrameGraph::Prepare()
{
    mDevice = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

}

RenderPass& FrameGraph::AddRenderPass(
    const std::string& label,
    RenderQueue queue)
{
    auto& it = mRenderPasses.find(label);
    if (it != mRenderPasses.end()) {
        return it->second;
    }

    mRenderPasses.emplace(label, RenderPass{});
    return mRenderPasses.find(label)->second;
}

void FrameGraph::ReconstructFrameGraph()
{
}