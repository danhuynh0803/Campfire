cmake_minimum_required(VERSION 3.15)

add_library(Core)

set(CORE_SRC
    Source/Core/Application.cpp
    #Source/Core/FileSystem.cpp
    Source/Core/Input.cpp
    Source/Core/Layer.cpp
    Source/Core/LayerStack.cpp
    Source/Core/Log.cpp
    Source/Core/Random.cpp
    #Source/Core/ResourceManager.cpp
    Source/Core/Time.cpp
    Source/Core/Window.cpp
)

set(CORE_HEADERS
    Include/Core/Application.h
    Include/Core/Base.h
    Include/Core/FileSystem.h
    Include/Core/Input.h
    Include/Core/Layer.h
    Include/Core/LayerStack.h
    Include/Core/Log.h
    Include/Core/MouseCodes.h
    Include/Core/Random.h
    #Include/Core/ResourceManager.h
    Include/Core/Time.h
    Include/Core/Timer.h
    Include/Core/Window.h
)

set_target_properties(Core PROPERTIES FOLDER "Engine")

target_include_directories(Core PUBLIC "Include/")

target_sources(Core
    PRIVATE
        ${CORE_SRC}
        ${CORE_HEADERS}
)

target_link_libraries(Core
    PUBLIC
        spdlog
        glm
        ImGui
        Events
        Renderer
        Windows
    PRIVATE
        glfw
        glad
)
