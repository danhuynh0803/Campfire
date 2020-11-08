cmake_minimum_required(VERSION 3.15)

add_library(Engine STATIC)

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
