find_package(Vulkan REQUIRED)

target_include_directories(
    Engine
    PUBLIC
        "include"
)

set(VULKAN_HEADERS
    include/Vulkan/VulkanBuffer.h
    include/Vulkan/VulkanContext.h
    include/Vulkan/VulkanDevice.h
    include/Vulkan/VulkanFramebuffer.h
    include/Vulkan/VulkanRenderer.h
    include/Vulkan/VulkanRendererAPI.h
    include/Vulkan/VulkanShader.h
    include/Vulkan/VulkanSwapChain.h
    #include/Vulkan/VulkanText.h
    include/Vulkan/VulkanTexture.h
    include/Vulkan/VulkanMaterial.h
    #include/Vulkan/VulkanVertexArray.h
    include/Vulkan/VulkanUtil.h
    include/Vulkan/VulkanInitializers.h
    include/Vulkan/VulkanMesh.h
    include/Vulkan/VulkanGraphicsPipeline.h
    include/Vulkan/VulkanComputePipeline.h
)

set(VULKAN_SRC
    src/VulkanBuffer.cpp
    src/VulkanContext.cpp
    src/VulkanDevice.cpp
    src/VulkanFramebuffer.cpp
    src/VulkanRenderer.cpp
    #src/VulkanRendererAPI.cpp
    src/VulkanShader.cpp
    src/VulkanSwapChain.cpp
    #src/VulkanText.cpp
    src/VulkanTexture.cpp
    #src/VulkanVertexArray.cpp
    src/VulkanUtil.cpp
    src/VulkanMesh.cpp
    src/VulkanGraphicsPipeline.cpp
    src/VulkanComputePipeline.cpp
)

target_sources(
    Engine
    PRIVATE
        ${VULKAN_SRC}
        ${VULKAN_HEADERS}
)

target_link_libraries(
    Engine
    PRIVATE
        Vulkan::Vulkan
        $ENV{VULKAN_SDK}/lib/shaderc_combinedd.lib
        # For release versions
        #$ENV{VULKAN_SDK}/lib/shaderc_combined.lib
)
