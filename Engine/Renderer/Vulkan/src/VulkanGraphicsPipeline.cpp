#include "Vulkan/VulkanGraphicsPipeline.h"
#include "Vulkan/VulkanShader.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanInitializers.h"

#include "Core/ResourceManager.h" // For asset directory macros

namespace
{
    enum class VertexComponent
    {
        Position,
        UV,
        Normal
    };

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec2 uv;
        glm::vec3 normal;

        static vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
        static vk::VertexInputBindingDescription vertexInputBindingDescription;
        static std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions;

        static vk::PipelineVertexInputStateCreateInfo* GetPipelineVertexInputState(const std::vector<VertexComponent>& components)
        {
            vertexInputBindingDescription = vk::initializers::VertexInputBindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex);

            uint32_t location = 0;
            for (auto component : components)
            {
                vertexInputAttributeDescriptions.push_back(Vertex::InputAttributeDescription(0, location, component));
                location++;
            }

            pipelineVertexInputStateCreateInfo = vk::initializers::PipelineVertexInputStateCreateInfo(
                1, &vertexInputBindingDescription,
                static_cast<uint32_t>(vertexInputAttributeDescriptions.size()),
                vertexInputAttributeDescriptions.data()
            );

            return &pipelineVertexInputStateCreateInfo;
        }

        static vk::VertexInputAttributeDescription InputAttributeDescription(
            uint32_t binding,
            uint32_t location,
            VertexComponent component
        )
        {
            switch (component)
            {
                case VertexComponent::Position:
                    return vk::initializers::VertexInputAttributeDescription(binding, location, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos));
                case VertexComponent::UV:
                    return vk::initializers::VertexInputAttributeDescription(binding, location, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv));
                case VertexComponent::Normal:
                    return vk::initializers::VertexInputAttributeDescription(binding, location, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal));
                default:
                    CORE_ERROR("No valid Input Attribute for specified component");
            }
        }
    };
}

vk::PipelineVertexInputStateCreateInfo Vertex::pipelineVertexInputStateCreateInfo;
std::vector<vk::VertexInputAttributeDescription> Vertex::vertexInputAttributeDescriptions;
vk::VertexInputBindingDescription Vertex::vertexInputBindingDescription;

VulkanGraphicsPipeline::VulkanGraphicsPipeline(
    const std::vector<std::vector<vk::DescriptorSetLayoutBinding>> & descriptorSetLayoutBindings
  , const std::vector<vk::PipelineShaderStageCreateInfo> & shaderStages
)
{
    mDevice = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

    // Input assembly
    auto inputAssemblyStateCreateInfo = vk::initializers::PipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology::eTriangleList, VK_FALSE);

    // Setup viewports and scissor rect
    auto swapChain = VulkanContext::Get()->GetSwapChain();
    auto swapChainExtent = swapChain->GetExtent();
    auto viewport = vk::initializers::Viewport(0.0f, 0.0f, swapChainExtent.width, swapChainExtent.height, 0.0f, 1.0);

    vk::Rect2D scissors;
    scissors.offset = {0, 0};
    scissors.extent = swapChainExtent;

    auto viewportStateCreateInfo = vk::initializers::PipelineViewportStateCreateInfo(1, &viewport, 1, &scissors);

    // Setup rasterizer
    auto rasterizationStateCreateInfo = vk::initializers::PipelineRasterizationStateCreateInfo(vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise);

    // Multisampling
    auto multisampleCreateInfo = vk::initializers::PipelineMultisampleStateCreateInfo(vk::SampleCountFlagBits::e1);

    // Depth and stencil operators
    auto depthStencilCreateInfo = vk::initializers::PipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, vk::CompareOp::eLess);

    // Setup color blending
    auto colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    auto colorBlendAttachment = vk::initializers::PipelineColorBlendAttachmentState(colorWriteMask, VK_FALSE);
    auto colorBlendState = vk::initializers::PipelineColorBlendStateCreateInfo(1, &colorBlendAttachment);

    // Setup dynamic state - these can be changed without recreating the pipeline
    std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eLineWidth };

    auto dynamicState = vk::initializers::PipelineDynamicStateCreateInfo(static_cast<uint32_t>(dynamicStates.size()), dynamicStates.data());

    // Create UniqueDescriptorSetLayouts from the layout bindings parameter
    for (auto setBindings : descriptorSetLayoutBindings)
    {
        auto descriptorSetLayoutInfo = vk::initializers::DescriptorSetLayoutCreateInfo(
            static_cast<uint32_t>(setBindings.size()),
            setBindings.data()
        );

        mDescriptorSetLayouts.emplace_back(
            mDevice.createDescriptorSetLayoutUnique(descriptorSetLayoutInfo)
        );
    }

    // DescriptorSets
    std::vector<vk::DescriptorSetLayout> setLayouts = vk::util::ConvertUnique(mDescriptorSetLayouts);
    const auto swapChainSize = VulkanContext::Get()->GetSwapChain()->GetImages().size();
    for (size_t i = 0; i < setLayouts.size(); ++i)
    {
        // Create 3 descriptor sets per each unique descriptorSetLayout
        // TODO: check if we actually need multiple? Would it suffice to just use
        // the same set per frame?
        std::vector<vk::DescriptorSetLayout> layouts { swapChainSize, setLayouts.at(i) };
        auto allocInfo = vk::initializers::DescriptorSetAllocateInfo(
            VulkanContext::Get()->GetDescriptorPool(),
            static_cast<uint32_t>(layouts.size()),
            layouts.data()
        );

        mDescriptorSets.emplace_back(mDevice.allocateDescriptorSetsUnique(allocInfo));
    }

    // Setup pipeline layout
    auto pipelineLayoutCreateInfo = vk::initializers::PipelineLayoutCreateInfo(
        static_cast<uint32_t>(setLayouts.size())
      , setLayouts.data()
    );

    vk::PushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
    pushConstantRange.size = sizeof(TransformPushConstBlock);
    pushConstantRange.offset = 0;

    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

    mPipelineLayout = mDevice.createPipelineLayoutUnique(pipelineLayoutCreateInfo);

    // Create Graphics pipeline
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.flags = vk::PipelineCreateFlags();
    pipelineCreateInfo.stageCount = shaderStages.size();
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.pVertexInputState = Vertex::GetPipelineVertexInputState({ VertexComponent::Position, VertexComponent::UV, VertexComponent::Normal });
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pDynamicState = nullptr;
    pipelineCreateInfo.layout = mPipelineLayout.get();
    pipelineCreateInfo.renderPass = VulkanContext::Get()->mFrameGraph.GetRenderPass("opaque");
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = nullptr;
    pipelineCreateInfo.basePipelineIndex = -1;

    mPipeline = mDevice.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo);
}

void VulkanGraphicsPipeline::RecreatePipeline()
{
    // TODO
    return;
}


