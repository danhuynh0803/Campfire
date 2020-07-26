cmake_minimum_required(VERSION 3.9)

project(Campfire VERSION 0.1
                 DESCRIPTION "Game Engine"
                 LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

# GLFW Options
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(Campfire/Vendor/glfw)

# ASSIMP Options
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(Campfire/Vendor/assimp)

# BULLET Options
option(BUILD_BULLET2_DEMOS OFF)
option(BUILD_CPU_DEMOS OFF)
option(BUILD_EXTRAS OFF)
option(BUILD_OPENGL3_DEMOS OFF)
option(BUILD_UNIT_TESTS OFF)
add_subdirectory(Campfire/Vendor/bullet)

if(MSVC)
    add_compile_options(/W4)
elseif(APPLE)
    SET(MACOSX_DEPLOYMENT_TARGET ${DARWIN_MAJOR_SDK_VERSION}.${DARWIN_MINOR_SDK_VERSION})
    SET(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")
    MESSAGE("Setting MACOSX_DEPLOYMENT_TARGET to '${MACOSX_DEPLOYMENT_TARGET}'.")
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

add_library(glad STATIC Campfire/Vendor/glad/src/glad.c)
target_link_libraries(glad PUBLIC Campfire/Vendor/glad/include/)

include_directories(Campfire/Sources/
                    Campfire/Vendor/assimp/include/
                    Campfire/Vendor/bullet/src/
                    Campfire/Vendor/glad/include/
                    Campfire/Vendor/glfw/include/
                    Campfire/Vendor/glm/
                    Campfire/Vendor/stb/
                    Campfire/Vendor/spdlog/include/
                    Campfire/Vendor/rapidjson/include
                    Campfire/Vendor/imgui
                    Campfire/Vendor/entt
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB PROJECT_SOURCES Campfire/Sources/*.h
                          Campfire/Sources/*.cpp)

file(GLOB SRC_CORE Campfire/Sources/Core/*.h
                   Campfire/Sources/Core/*.cpp)

file(GLOB SRC_EVENTS Campfire/Sources/Events/*.h
                     Campfire/Sources/Events/*.cpp)

file(GLOB SRC_PLATFORM_OPENGL Campfire/Sources/Platform/OpenGL/*.h
                              Campfire/Sources/Platform/OpenGL/*.cpp)

file(GLOB SRC_PLATFORM_WINDOWS Campfire/Sources/Platform/Windows/*.h
                               Campfire/Sources/Platform/Windows/*.cpp)

file(GLOB SRC_PLATFORM_LINUX Campfire/Sources/Platform/LINUX/*.h
                             Campfire/Sources/Platform/LINUX/*.cpp)

file(GLOB SRC_RENDERER Campfire/Sources/Renderer/*.h
                       Campfire/Sources/Renderer/*.cpp)

file(GLOB SRC_SCENE Campfire/Sources/Scene/*.h
                    Campfire/Sources/Scene/*.cpp)

file(GLOB SRC_PARTICLES Campfire/Sources/Particles/*.h
                        Campfire/Sources/Particles/*.cpp)

file(GLOB SRC_IMGUI Campfire/Sources/ImGui/*.h
                    Campfire/Sources/ImGui/*.cpp)

file(GLOB PROJECT_SHADERS Campfire/Shaders/*.comp
                          Campfire/Shaders/*.frag
                          Campfire/Shaders/*.geom
                          Campfire/Shaders/*.vert)

file(GLOB VENDORS_SOURCES Campfire/Vendor/glad/src/glad.c
                          Campfire/Vendor/imgui/*.cpp
                          Campfire/Vendor/tracy/TracyClient.cpp)

file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)


# Source groupings
source_group("Sources"                     FILES ${PROJECT_SOURCES})
source_group("Sources\\Core"               FILES ${SRC_CORE})
source_group("Sources\\Events"             FILES ${SRC_EVENTS})
source_group("Sources\\Platform\\OpenGL"   FILES ${SRC_PLATFORM_OPENGL})
source_group("Sources\\Platform\\Windows"  FILES ${SRC_PLATFORM_WINDOWS})
source_group("Sources\\Platform\\Linux"    FILES ${SRC_PLATFORM_LINUX})
source_group("Sources\\Renderer"           FILES ${SRC_RENDERER})
source_group("Sources\\Particles"          FILES ${SRC_PARTICLES})
source_group("Sources\\Scene"              FILES ${SRC_SCENE})
source_group("Sources\\ImGui"              FILES ${SRC_IMGUI})

# Shaders
source_group("Shaders" FILES ${PROJECT_SHADERS})

# Vendor
source_group("Vendors" FILES ${VENDORS_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES}
                               ${SRC_CORE}
                               ${SRC_EVENTS}
                               ${SRC_PLATFORM_OPENGL}
                               ${SRC_PLATFORM_WINDOWS}
                               ${SRC_PLATFORM_LINUX}
                               ${SRC_RENDERER}
                               ${SRC_PARTICLES}
                               ${SRC_SCENE}
                               ${SRC_IMGUI}
                               ${PROJECT_SHADERS}
                               ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES}
)

target_link_libraries(${PROJECT_NAME} assimp glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
                      BulletDynamics BulletCollision LinearMath)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
