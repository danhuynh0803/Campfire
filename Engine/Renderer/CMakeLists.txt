add_library(Renderer)

target_include_directories(Renderer PUBLIC "Include/")

target_sources(Renderer
    PRIVATE
        #Source/Renderer/Buffer.cpp
        #Source/Renderer/Framebuffer.cpp
        Source/Renderer/GraphicsContext.cpp
        #Source/Renderer/Material.cpp
        #Source/Renderer/Mesh.cpp
        #Source/Renderer/RenderCommand.cpp
        #Source/Renderer/Renderer.cpp
        #Source/Renderer/Renderer2D.cpp
        #Source/Renderer/RendererAPI.cpp
        #Source/Renderer/SceneRenderer.cpp
        #Source/Renderer/Shader.cpp
        #Source/Renderer/Text.cpp
        #Source/Renderer/Texture.cpp
        #Source/Renderer/VertexArray.cpp
)

target_link_libraries(Renderer
    PRIVATE
        Core
        OpenGLImpl
)

add_subdirectory(OpenGL)
#add_subdirectory(Vulkan)
