cmake_minimum_required(VERSION 3.15)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(Engine SHARED)

set_target_properties(Engine PROPERTIES FOLDER "Engine")

# for including EntryPoint.h
target_include_directories(Engine PUBLIC "./")

target_link_libraries(Engine
    PUBLIC
        Core
        glfw
        Events
        Windows
        Renderer
)

# Create a empty file just to tell VS to make the lib
file(WRITE null.cpp "")

target_sources(Engine
    PUBLIC
        EntryPoint.h
    PRIVATE
        null.cpp
)

add_subdirectory(Core)
add_subdirectory(Events)
add_subdirectory(ImGui)
add_subdirectory(Platform)
add_subdirectory(Renderer)
add_subdirectory(Scene)
