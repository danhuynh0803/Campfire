cmake_minimum_required(VERSION 3.15)

add_library(Core)

target_include_directories(Core PUBLIC "Include/")

target_sources(Core
    PRIVATE
        Source/Core/Application.cpp
        Source/Core/FileSystem.cpp
        Source/Core/Input.cpp
        Source/Core/Layer.cpp
        Source/Core/LayerStack.cpp
        Source/Core/Log.cpp
        Source/Core/Random.cpp
        Source/Core/ResourceManager.cpp
        Source/Core/Time.cpp
        Source/Core/Window.cpp
)

target_link_libraries(Core
    PRIVATE
        Events
)
