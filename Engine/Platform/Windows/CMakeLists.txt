cmake_minimum_required(VERSION 3.15)

add_library(Windows)

set_target_properties(Windows PROPERTIES FOLDER "Engine/Platform/")

target_include_directories(Windows PUBLIC "Include/")

set(WINDOWS_SRC
    Source/Windows/WindowsFileSystem.cpp
    Source/Windows/WindowsWindow.cpp
)

set(WINDOWS_HEADERS
    Include/Platform/Windows/WindowsFileSystem.h
    Include/Platform/Windows/WindowsWindow.h
)

target_sources(Windows
    PRIVATE
        ${WINDOWS_SRC}
        ${WINDOWS_HEADERS}
)

target_link_libraries(Windows
    PUBLIC
        Renderer
    PRIVATE
        glfw
        glad
        Core
        Events
)
