cmake_minimum_required(VERSION 3.15)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(Engine STATIC)

set_target_properties(Engine PROPERTIES FOLDER "Engine")

# for including EntryPoint.h
target_include_directories(Engine PUBLIC "./")

target_link_libraries(Engine
    PUBLIC
        Core
    PRIVATE
        Events
        Windows
        Renderer
)

target_sources(Engine
    PUBLIC
        EntryPoint.h
)

add_subdirectory(Core)
add_subdirectory(Events)
add_subdirectory(Platform)
add_subdirectory(Renderer)
