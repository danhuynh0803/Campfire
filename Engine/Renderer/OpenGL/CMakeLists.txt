target_include_directories(
    Engine
    PUBLIC
        "include"
)

set(OPENGL_HEADERS
    include/OpenGL/OpenGLBuffer.h
    include/OpenGL/OpenGLContext.h
    include/OpenGL/OpenGLFramebuffer.h
    include/OpenGL/OpenGLRendererAPI.h
    include/OpenGL/OpenGLShader.h
    include/OpenGL/OpenGLText.h
    include/OpenGL/OpenGLTexture.h
    include/OpenGL/OpenGLVertexArray.h
)

set(OPENGL_SRC
    src/OpenGL/OpenGLBuffer.cpp
    src/OpenGL/OpenGLContext.cpp
    src/OpenGL/OpenGLFramebuffer.cpp
    src/OpenGL/OpenGLRendererAPI.cpp
    src/OpenGL/OpenGLShader.cpp
    src/OpenGL/OpenGLText.cpp
    src/OpenGL/OpenGLTexture.cpp
    src/OpenGL/OpenGLVertexArray.cpp
)

target_sources(
    Engine
    PRIVATE
        ${OPENGL_SRC}
        ${OPENGL_HEADERS}
)
