add_library(Renderer)

set_target_properties(Renderer PROPERTIES FOLDER "Engine/")

target_include_directories(Renderer PUBLIC "Include/")

set(RENDERER_SRC
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

set(RENDERER_HEADERS
    #Include/Renderer/Buffer.h
    #Include/Renderer/Framebuffer.h
    Include/Renderer/GraphicsContext.h
    #Include/Renderer/Material.h
    #Include/Renderer/Mesh.h
    #Include/Renderer/RenderCommand.h
    #Include/Renderer/Renderer.h
    #Include/Renderer/Renderer2D.h
    #Include/Renderer/RendererAPI.h
    #Include/Renderer/SceneRenderer.h
    #Include/Renderer/Shader.h
    #Include/Renderer/Text.h
    #Include/Renderer/Texture.h
    #Include/Renderer/VertexArray.h
)


target_sources(Renderer
    PRIVATE
        ${RENDERER_SRC}
        ${RENDERER_HEADERS}
)

target_link_libraries(Renderer
    PRIVATE
        Core
        OpenGLImpl
)

add_subdirectory(OpenGL)
#add_subdirectory(Vulkan)
