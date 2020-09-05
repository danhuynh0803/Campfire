cmake_minimum_required(VERSION 3.9)

project(Campfire)
#project(Campfire VERSION 0.1
#                 DESCRIPTION "Game Engine"
#                 LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

# LUA Options
add_subdirectory(Campfire/Vendor/lua-5.4.0)

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


add_subdirectory(Campfire/Vendor/freetype2)


# FMOD
set(FMOD_DIR "${CMAKE_SOURCE_DIR}/Campfire/Vendor/fmod")
#find_path (FMOD_INCLUDE_DIR fmod.h)
if(MSVC)
    # FIXME
    find_library(FMOD_LIB fmod_vc)
    find_library(FMOD_STUDIO_LIB fmodstudio_vc)
    include_directories(
        ${FMOD_DIR}/Windows/core/inc/
        ${FMOD_DIR}/Windows/studio/inc/
        )

    link_directories(
        ${FMOD_DIR}/Windows/core/lib/
        ${FMOD_DIR}/Windows/studio/lib/
        )
else()
    # FIXME
    find_library(FMOD_LIB fmod)
    find_library(FMOD_STUDIO_LIB fmodstudio)

    include_directories(
        ${FMOD_DIR}/Linux/core/inc/
        ${FMOD_DIR}/Linux/studio/inc/
        )

    link_directories(
        ${FMOD_DIR}/Linux/core/lib/x86_64/
        ${FMOD_DIR}/Linux/studio/lib/x86_64/
        )
endif()


if(MSVC)
    add_compile_options(/W4)
    add_compile_options(/D TRACY_ENABLE)
    # For multidll projects
    #add_compile_options(/D TRACY_IMPORTS)
elseif(APPLE)
    SET(MACOSX_DEPLOYMENT_TARGET ${DARWIN_MAJOR_SDK_VERSION}.${DARWIN_MINOR_SDK_VERSION})
    SET(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")
    MESSAGE("Setting MACOSX_DEPLOYMENT_TARGET to '${MACOSX_DEPLOYMENT_TARGET}'.")
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    #TODO tracy compile option for linux
    #find_package(PkgConfig REQUIRED)
    #PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)
    #include_directories(${GTK3_INCLUDE_DIRS})
    #link_directories(${GTK3_LIBRARY_DIRS})
    #add_definitions(${GTK3_CFLAGS_OTHER})
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

include_directories(
    Campfire/Sources/
    Campfire/Vendor/assimp/include/
    Campfire/Vendor/bullet/src/
    Campfire/Vendor/glad/include/
    Campfire/Vendor/glfw/include/
    Campfire/Vendor/glm/
    Campfire/Vendor/stb/
    Campfire/Vendor/spdlog/include/
    Campfire/Vendor/rapidjson/include/
    Campfire/Vendor/rapidjson/include/
    Campfire/Vendor/imgui
    Campfire/Vendor/entt
    Campfire/Vendor/tracy
    Campfire/Vendor/freetype2/include/
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

file(GLOB SRC_RENDERER Campfire/Sources/Renderer/*.h
                       Campfire/Sources/Renderer/*.cpp)

file(GLOB SRC_SCENE Campfire/Sources/Scene/*.h
                    Campfire/Sources/Scene/*.cpp)

file(GLOB SRC_PARTICLES Campfire/Sources/Particles/*.h
                        Campfire/Sources/Particles/*.cpp)

file(GLOB SRC_PHYSICS Campfire/Sources/Physics/*.h
                      Campfire/Sources/Physics/*.cpp)

file(GLOB SRC_IMGUI Campfire/Sources/ImGui/*.h
                    Campfire/Sources/ImGui/*.cpp)

file(GLOB SRC_AUDIO Campfire/Sources/Audio/*.h
                    Campfire/Sources/Audio/*.cpp)

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

file(GLOB SRC_SCRIPTING Campfire/Sources/Scripting/*.h
                   Campfire/Sources/Scripting/*.cpp)

file(GLOB SRC_EDITOR
    Campfire/Sources/Editor/*.h
    Campfire/Sources/Editor/*.cpp
)

file(GLOB SRC_EDITOR_WIDGETS
    Campfire/Sources/Editor/Widgets/*.h
    Campfire/Sources/Editor/Widgets/*.cpp
)

if(MSVC)
file(GLOB SRC_PLATFORM_WINDOWS Campfire/Sources/Platform/Windows/*.h
                               Campfire/Sources/Platform/Windows/*.cpp)

source_group("Sources\\Platform\\Windows"  FILES ${SRC_PLATFORM_WINDOWS})
endif()

if (NOT MSVC)
file(GLOB SRC_PLATFORM_LINUX Campfire/Sources/Platform/Linux/*.h
                             Campfire/Sources/Platform/ Campfire/Sources/Platform/Linux/*.cpp)

source_group("Sources\\Platform\\Linux"    FILES ${SRC_PLATFORM_LINUX})
endif()


# Source groupings for IDEs
source_group("Sources"                     FILES ${PROJECT_SOURCES})
source_group("Sources\\Core"               FILES ${SRC_CORE})
source_group("Sources\\Events"             FILES ${SRC_EVENTS})
source_group("Sources\\Platform\\OpenGL"   FILES ${SRC_PLATFORM_OPENGL})
source_group("Sources\\Renderer"           FILES ${SRC_RENDERER})
source_group("Sources\\Particles"          FILES ${SRC_PARTICLES})
source_group("Sources\\Physics"            FILES ${SRC_PHYSICS})
source_group("Sources\\Scene"              FILES ${SRC_SCENE})
source_group("Sources\\ImGui"              FILES ${SRC_IMGUI})
source_group("Sources\\Audio"              FILES ${SRC_AUDIO})
source_group("Sources\\Scripting"          FILES ${SRC_SCRIPTING})
source_group("Sources\\Editor"             FILES ${SRC_EDITOR})
source_group("Sources\\Editor\\Widgets"    FILES ${SRC_EDITOR_WIDGETS})

# Shaders
source_group("Shaders" FILES ${PROJECT_SHADERS})

# Vendor
source_group("Vendors" FILES ${VENDORS_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

add_executable(
    ${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${SRC_CORE}
        ${SRC_EVENTS}
        ${SRC_PLATFORM_OPENGL}
        ${SRC_PLATFORM_WINDOWS}
        ${SRC_PLATFORM_LINUX}
        ${SRC_RENDERER}
        ${SRC_PARTICLES}
        ${SRC_PHYSICS}
        ${SRC_SCENE}
        ${SRC_IMGUI}
        ${SRC_AUDIO}
        ${SRC_SCRIPTING}
        ${SRC_EDITOR}
        ${SRC_EDITOR_WIDGETS}
        ${PROJECT_SHADERS}
        ${PROJECT_CONFIGS}
        ${VENDORS_SOURCES}
)


target_link_libraries(
    ${PROJECT_NAME}
        ${GLFW_LIBRARIES}
        ${GLAD_LIBRARIES}
        assimp
        glfw
        BulletDynamics BulletCollision LinearMath
        LuaLib
        freetype
)

if(MSVC)
    target_link_libraries(${PROJECT_NAME} fmod_vc fmodstudio_vc)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_DIR}/Windows/core/lib/fmod.dll ${FMOD_DIR}/Windows/studio/lib/fmodstudio.dll ${CMAKE_BINARY_DIR}/. )
else()
    target_link_libraries(${PROJECT_NAME} fmod fmodstudio)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_DIR}/Linux/core/lib/x86_64/libfmod.so ${FMOD_DIR}/Linux/studio/lib/x86_64/libfmodstudio.so ${CMAKE_BINARY_DIR}/. )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
