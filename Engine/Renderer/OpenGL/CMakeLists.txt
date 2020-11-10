add_library(OpenGLImpl STATIC)

set_target_properties(OpenGLImpl PROPERTIES FOLDER "Engine/Rendering/APIs/")

target_include_directories(OpenGLImpl PUBLIC "Include/")

set(OPENGL_SRC
    Source/OpenGL/OpenGLBuffer.cpp
    Source/OpenGL/OpenGLContext.cpp
    Source/OpenGL/OpenGLFramebuffer.cpp
    Source/OpenGL/OpenGLRendererAPI.cpp
    Source/OpenGL/OpenGLShader.cpp
    #Source/OpenGL/OpenGLText.cpp
    Source/OpenGL/OpenGLTexture.cpp
    Source/OpenGL/OpenGLVertexArray.cpp
)

set(OPENGL_HEADERS
    Include/OpenGL/OpenGLBuffer.h
    Include/OpenGL/OpenGLContext.h
    Include/OpenGL/OpenGLFramebuffer.h
    Include/OpenGL/OpenGLRendererAPI.h
    Include/OpenGL/OpenGLShader.h
    #Include/OpenGL/OpenGLText.h
    Include/OpenGL/OpenGLTexture.h
    Include/OpenGL/OpenGLVertexArray.h
)

target_sources(OpenGLImpl
    PRIVATE
        ${OPENGL_SRC}
        ${OPENGL_HEADERS}
)

target_link_libraries(OpenGLImpl
    PUBLIC
        Core
        glfw
        glad
        stb
)
