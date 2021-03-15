#include "ImGui/VulkanImGui.h"
#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanShader.h"
#include "Core/ResourceManager.h"

VulkanImGuiImpl::VulkanImGuiImpl()
{
}

void VulkanImGuiImpl::InitResources()
{
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    ImGuiIO& io = ImGui::GetIO();

    // Create font texture
    unsigned char* fontData;
    int texWidth, texHeight;
    io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
    vk::DeviceSize size = texWidth * texHeight * 4 * sizeof(char);

    // Create target image for copy
    mFontImage = vkUtil::CreateUniqueImage(
        texWidth, texHeight,
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst
    );

    // Allocate memory to image
    mFontMemory = vkUtil::CreateUniqueDeviceMemory(
        mFontImage.get(),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );
    device.bindImageMemory(mFontImage.get(), mFontMemory.get(), 0);

    // Create image view
    mFontImageView = vkUtil::CreateUniqueImageView(
        mFontImage.get(),
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageAspectFlagBits::eColor
    );

    // Staging buffer for font data upload
    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.size = size;
    bufferInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;
    vk::UniqueBuffer stagingBuffer = device.createBufferUnique(bufferInfo);

    vk::MemoryRequirements memoryReqs = device.getBufferMemoryRequirements(stagingBuffer.get());
    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memoryReqs.size;
    allocInfo.memoryTypeIndex = vkUtil::FindMemoryType(
        memoryReqs.memoryTypeBits,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    );

    vk::UniqueDeviceMemory stagingBufferMemory = device.allocateMemoryUnique(allocInfo);
    device.bindBufferMemory(stagingBuffer.get(), stagingBufferMemory.get(), 0);

    void* data = device.mapMemory(stagingBufferMemory.get(), 0, size, vk::MemoryMapFlags());
        memcpy(data, fontData, size);
    device.unmapMemory(stagingBufferMemory.get());

    // Prepare for transfer, switch image layout
    vkUtil::SwitchImageLayout(
        mFontImage.get(),
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal
    );

    // Copy data from stagingBuffer to font image
    auto cmdBuffer = vkUtil::BeginSingleTimeCommands();
        vk::BufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageExtent = {
            static_cast<uint32_t>(texWidth),
            static_cast<uint32_t>(texHeight),
            1
        };

        cmdBuffer.copyBufferToImage(
            stagingBuffer.get(),
            mFontImage.get(),
            vk::ImageLayout::eTransferDstOptimal,
            1,
            &region
        );
    vkUtil::EndSingleTimeCommands(cmdBuffer);

    // Transition image for shader read
    vkUtil::SwitchImageLayout(
        mFontImage.get(),
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal
    );

    // Create font texture sampler
    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.magFilter    = vk::Filter::eLinear;
    samplerInfo.minFilter    = vk::Filter::eLinear;
    samplerInfo.mipmapMode   = vk::SamplerMipmapMode::eLinear;
    samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.borderColor  = vk::BorderColor::eFloatOpaqueWhite;

    mSampler = device.createSamplerUnique(samplerInfo);

    { // Create descriptor pool
        std::vector<vk::DescriptorPoolSize> poolSizes =
        {
            { vk::DescriptorType::eCombinedImageSampler, 1},
        };
        vk::DescriptorPoolCreateInfo descriptorPoolInfo{};
        //descriptorPoolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        mDescriptorPool = device.createDescriptorPoolUnique(descriptorPoolInfo);
    }

    { // Descriptor Set Layout
        std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
        vk::DescriptorSetLayoutBinding samplerBinding;
        samplerBinding.binding = 0;
        samplerBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        samplerBinding.descriptorCount = 1;
        samplerBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;
        layoutBindings.emplace_back(samplerBinding);

        vk::DescriptorSetLayoutCreateInfo descriptorLayoutInfo{};
        descriptorLayoutInfo.bindingCount = 1;
        descriptorLayoutInfo.pBindings = layoutBindings.data();

        mDescriptorSetLayout = device.createDescriptorSetLayoutUnique(descriptorLayoutInfo);
    }

    { // Create Descriptor Set
        vk::DescriptorSetAllocateInfo setInfo;
        setInfo.descriptorPool = mDescriptorPool.get();
        setInfo.descriptorSetCount = 1;
        setInfo.pSetLayouts = &mDescriptorSetLayout.get();
        mDescriptorSets = device.allocateDescriptorSetsUnique(setInfo);

        // TODO investigate what is this used for
        // DescriptorImageInfo
        vk::DescriptorImageInfo imageInfo{};
        imageInfo.sampler = mSampler.get();
        imageInfo.imageView = mFontImageView.get();
        imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
        vk::WriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.dstSet = mDescriptorSets.at(0).get();
        writeDescriptorSet.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        writeDescriptorSet.dstBinding = 1;
        writeDescriptorSet.pImageInfo = &imageInfo;

        writeDescriptorSets.emplace_back(writeDescriptorSet);

        // Update descriptor sets
        device.updateDescriptorSets(
            static_cast<uint32_t>(writeDescriptorSets.size()),
            writeDescriptorSets.data(),
            0,
            nullptr
        );
    }

    // Create pipeline cache
    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    mPipelineCache = device.createPipelineCacheUnique(pipelineCacheCreateInfo);

    { // Pipeline layout
        vk::PushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
        pushConstantRange.size = sizeof(vk::PushConstantRange);
        pushConstantRange.offset = 0;

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        mPipelineLayout = device.createPipelineLayoutUnique(pipelineLayoutInfo);
    }

    // Graphics pipeline for UI rendering
    // Input assembly state
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {};
    inputAssemblyStateInfo.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssemblyStateInfo.primitiveRestartEnable = false;

    // Rasterization state
    vk::PipelineRasterizationStateCreateInfo rasterizationStateInfo = {};
    rasterizationStateInfo.polygonMode = vk::PolygonMode::eFill;
    rasterizationStateInfo.cullMode = vk::CullModeFlagBits::eNone;
    rasterizationStateInfo.frontFace = vk::FrontFace::eCounterClockwise;
    rasterizationStateInfo.depthClampEnable = false;
    rasterizationStateInfo.lineWidth = 1.0f;

    // Enable Blending
    vk::PipelineColorBlendAttachmentState blendAttachmentState = {};
    blendAttachmentState.blendEnable = true;
    blendAttachmentState.colorWriteMask =
        vk::ColorComponentFlagBits::eR
        | vk::ColorComponentFlagBits::eG
        | vk::ColorComponentFlagBits::eB
        | vk::ColorComponentFlagBits::eA
        ;
    blendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    blendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    blendAttachmentState.colorBlendOp = vk::BlendOp::eAdd;
    blendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    blendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    blendAttachmentState.alphaBlendOp = vk::BlendOp::eAdd;

    // Color blend state
    vk::PipelineColorBlendStateCreateInfo colorBlendInfo = {};
    colorBlendInfo.attachmentCount = 1;
    colorBlendInfo.pAttachments = &blendAttachmentState;

    // Depth stencil state (not enabled, but still need to create)
    vk::PipelineDepthStencilStateCreateInfo depthStencilInfo = {};
    depthStencilInfo.depthTestEnable = false;
    depthStencilInfo.depthWriteEnable = false;
    depthStencilInfo.depthCompareOp = vk::CompareOp::eLessOrEqual;
    depthStencilInfo.back.compareOp = vk::CompareOp::eAlways;

    // Viewport
    vk::PipelineViewportStateCreateInfo viewportInfo = {};
    viewportInfo.viewportCount = 1;
    viewportInfo.scissorCount = 1;
    viewportInfo.flags = vk::PipelineViewportStateCreateFlags();

    // Multisample
    vk::PipelineMultisampleStateCreateInfo multisampleInfo = {};
    multisampleInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;

    // Dynamic state
    std::vector<vk::DynamicState> dynamicStateEnables = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };
    vk::PipelineDynamicStateCreateInfo dynamicStateInfo = {};
    dynamicStateInfo.pDynamicStates = dynamicStateEnables.data();
    dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

    // shader stages
    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStageInfos{};

    // Create pipeline
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleInfo;
    pipelineCreateInfo.pViewportState = &viewportInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStateInfo;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStageInfos.size());
    pipelineCreateInfo.pStages = shaderStageInfos.data();

    // Setup vertex bindings
    vk::VertexInputBindingDescription vertexInputBinding = {};
    vertexInputBinding.binding = 0;
    vertexInputBinding.stride = sizeof(ImDrawVert);
    vertexInputBinding.inputRate = vk::VertexInputRate::eVertex;
    std::vector<vk::VertexInputBindingDescription> vertexInputBindings = {
        vertexInputBinding
    };

    // Setup vertex attributes
    std::array<vk::VertexInputAttributeDescription, 3> vertexAttributes{};

    // Position
    vertexAttributes[0].binding = 0;
    vertexAttributes[0].location = 0;
    vertexAttributes[0].format = vk::Format::eR32G32Sfloat;
    vertexAttributes[0].offset = offsetof(ImDrawVert, pos);

    // UV
    vertexAttributes[1].binding = 0;
    vertexAttributes[1].location = 1;
    vertexAttributes[1].format = vk::Format::eR32G32Sfloat;
    vertexAttributes[1].offset = offsetof(ImDrawVert, uv);

    // Color
    vertexAttributes[2].binding = 0;
    vertexAttributes[2].location = 2;
    vertexAttributes[2].format = vk::Format::eR8G8B8A8Unorm;;
    vertexAttributes[2].offset = offsetof(ImDrawVert, col);


    // Vertex input state
    vk::PipelineVertexInputStateCreateInfo vertexInputStateInfo = {};
    vertexInputStateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
    vertexInputStateInfo.pVertexBindingDescriptions = vertexInputBindings.data();
    vertexInputStateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size());
    vertexInputStateInfo.pVertexAttributeDescriptions = vertexAttributes.data();

    pipelineCreateInfo.pVertexInputState = &vertexInputStateInfo;

    VulkanShader vert(SHADERS + "/ui.vert.spv");
    VulkanShader frag(SHADERS + "/ui.frag.spv");

    // TODO refactor this to be inside VulkanShader
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = vert.GetShaderModule();
    vertShaderStageInfo.pName = "ui";

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
    fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = frag.GetShaderModule();
    fragShaderStageInfo.pName = "ui";

    shaderStageInfos[0] = vertShaderStageInfo;
    shaderStageInfos[1] = fragShaderStageInfo;

    mPipeline = device.createGraphicsPipelineUnique(
        mPipelineCache.get(),
        pipelineCreateInfo
    );
}
