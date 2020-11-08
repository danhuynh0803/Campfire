cmake_minimum_required(VERSION 3.15)

add_library(Engine STATIC)

target_sources(Engine
    PRIVATE
        main.cpp
)

target_link_libraries(Engine
    PRIVATE
        Core
        Events
)

add_subdirectory(Core)
add_subdirectory(Events)
