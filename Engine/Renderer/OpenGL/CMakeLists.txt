add_library(OpenGLImpl)

target_include_directories(OpenGLImpl PUBLIC "Include/")

target_sources(OpenGLImpl
    PRIVATE
        #Source/OpenGL/OpenGLBuffer.cpp
        Source/OpenGL/OpenGLContext.cpp
        #Source/OpenGL/OpenGLFramebuffer.cpp
        #Source/OpenGL/OpenGLRendererAPI.cpp
        #Source/OpenGL/OpenGLShader.cpp
        #Source/OpenGL/OpenGLText.cpp
        #Source/OpenGL/OpenGLTexture.cpp
        #Source/OpenGL/OpenGLVertexArray.cpp
)

target_link_libraries(OpenGLImpl
    PUBLIC
        glfw
    PRIVATE
        glad
        Core
)
