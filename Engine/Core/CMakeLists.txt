target_include_directories(
    Engine
    PUBLIC
        "include"
)

set(CORE_HEADERS
    include/Core/Application.h
    include/Core/Base.h
    include/Core/FileSystem.h
    include/Core/Input.h
    include/Core/Layer.h
    include/Core/LayerStack.h
    include/Core/Log.h
    include/Core/MouseCodes.h
    include/Core/Random.h
    include/Core/ResourceManager.h
    include/Core/Time.h
    include/Core/Timer.h
    include/Core/Window.h
)

set(CORE_SRC
    src/Core/Application.cpp
    src/Core/FileSystem.cpp
    src/Core/Input.cpp
    src/Core/Layer.cpp
    src/Core/LayerStack.cpp
    src/Core/Log.cpp
    src/Core/Random.cpp
    src/Core/ResourceManager.cpp
    src/Core/Time.cpp
    src/Core/Window.cpp
)

source_group(
    "${VS_INC_DIR}\\Core"
    FILES ${CORE_HEADERS}
)

source_group(
    "${VS_SRC_DIR}\\Core"
    FILES ${CORE_SRC}
)

target_sources(Engine
    PRIVATE
        ${CORE_SRC}
        ${CORE_HEADERS}
)
