cmake_minimum_required(VERSION 3.15)

add_library(Windows)

target_include_directories(Windows PUBLIC "Include/")

target_sources(Windows
    PRIVATE
        Source/Windows/WindowsFileSystem.cpp
        Source/Windows/WindowsWindow.cpp
)

target_link_libraries(Windows
    PUBLIC
        Renderer
    PRIVATE
        glfw
        Core
        Events
)
