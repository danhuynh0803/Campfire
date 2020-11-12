cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(Campfire
    VERSION 0.1
    DESCRIPTION "Game Engine"
    LANGUAGES CXX
)

add_subdirectory(Engine)
add_subdirectory(Vendor)
add_subdirectory(Editor)
add_subdirectory(Game)
