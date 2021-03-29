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
    include/Vulkan/VulkanPipeline.h
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
)

set(VULKAN_SRC
    src/VulkanBuffer.cpp
    src/VulkanContext.cpp
    src/VulkanDevice.cpp
    src/VulkanFramebuffer.cpp
    src/VulkanPipeline.cpp
    src/VulkanRenderer.cpp
    #src/VulkanRendererAPI.cpp
    src/VulkanShader.cpp
    src/VulkanSwapChain.cpp
    #src/VulkanText.cpp
    src/VulkanTexture.cpp
    #src/VulkanVertexArray.cpp
    src/VulkanUtil.cpp
    src/VulkanMesh.cpp
)

target_sources(
    Engine
    PRIVATE
        ${VULKAN_SRC}
        ${VULKAN_HEADERS}
)

target_link_libraries(
    Engine
    PUBLIC
        Vulkan::Vulkan
)
