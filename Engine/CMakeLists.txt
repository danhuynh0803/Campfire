cmake_minimum_required(VERSION 3.15)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(Engine STATIC)

set_target_properties(Engine PROPERTIES FOLDER "Engine")

target_link_libraries(Engine
    PUBLIC
        Core
    PRIVATE
        Events
        Windows
        Renderer
)

# dummy file since we can't create a lib without some source
file(WRITE null.cpp "")

target_sources(Engine
    PRIVATE
        null.cpp
)

add_subdirectory(Core)
add_subdirectory(Events)
add_subdirectory(Platform)
add_subdirectory(Renderer)
