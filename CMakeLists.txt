cmake_minimum_required(VERSION 3.0)
project(Campfire)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(Campfire/Vendor/glfw)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(Campfire/Vendor/assimp)

option(BUILD_BULLET2_DEMOS OFF)
option(BUILD_CPU_DEMOS OFF)
option(BUILD_EXTRAS OFF)
option(BUILD_OPENGL3_DEMOS OFF)
option(BUILD_UNIT_TESTS OFF)
add_subdirectory(Campfire/Vendor/bullet)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif(APPLE)
    SET(MACOSX_DEPLOYMENT_TARGET ${DARWIN_MAJOR_SDK_VERSION}.${DARWIN_MINOR_SDK_VERSION})
    SET(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")
    MESSAGE("Setting MACOSX_DEPLOYMENT_TARGET to '${MACOSX_DEPLOYMENT_TARGET}'.")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

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

file(GLOB VENDORS_SOURCES Campfire/Vendor/glad/src/glad.c
                          Campfire/Vendor/imgui/*.cpp
                          Campfire/Vendor/tracy/TracyClient.cpp
)

file(GLOB PROJECT_HEADERS Campfire/Headers/*.hpp)

file(GLOB PROJECT_SOURCES Campfire/Sources/Core/*.h
                          Campfire/Sources/Core/*.cpp
                          Campfire/Sources/Events/*.h
                          Campfire/Sources/Events/*.cpp
                          Campfire/Sources/Platform/OpenGL/*.h
                          Campfire/Sources/Platform/OpenGL/*.cpp
                          Campfire/Sources/Platform/Windows/*.h
                          Campfire/Sources/Platform/Windows/*.cpp
                          Campfire/Sources/Platform/Linux/*.h
                          Campfire/Sources/Platform/Linux/*.cpp
                          Campfire/Sources/Renderer/*.h
                          Campfire/Sources/Renderer/*.cpp
                          #Campfire/Sources/Physics/*.h
                          #Campfire/Sources/Physics/*.cpp
                          Campfire/Sources/JobSystem/*.h
                          Campfire/Sources/JobSystem/*.cpp
                          Campfire/Sources/ImGui/*.h
                          Campfire/Sources/ImGui/*.cpp
                          Campfire/Sources/Scene/*.h
                          Campfire/Sources/Scene/*.cpp
                          Campfire/Sources/Particles/*.h
                          Campfire/Sources/Particles/*.cpp
                          Campfire/Sources/*.h
                          Campfire/Sources/*.cpp
)

#file(GLOB PROJECT_SOURCES Campfire/Sources/*.h
#                          Campfire/Sources/*.cpp
#                          Campfire/Sources/imgui/*.h
#                          Campfire/Sources/imgui/*.cpp
#)
#file(GLOB SRC_CORE Campfire/Sources/Core/*.h
#                   Campfire/Sources/Core/*.cpp
#)
#file(GLOB SRC_EVENTS Campfire/Sources/Events/*.h
#                     Campfire/Sources/Events/*.cpp
#)
#file(GLOB SRC_PLATFORM_OPENGL Campfire/Sources/Platform/OpenGL/*.h
#                              Campfire/Sources/Platform/OpenGL/*.cpp
#)
#file(GLOB SRC_PLATFORM_WINDOWS Campfire/Sources/Platform/Windows/*.h
#                               Campfire/Sources/Platform/Windows/*.cpp
#)
#file(GLOB SRC_RENDERER Campfire/Sources/Renderer/*.h
#                       Campfire/Sources/Renderer/*.cpp
#)

file(GLOB PROJECT_SHADERS Campfire/Shaders/*.comp
                          Campfire/Shaders/*.frag
                          Campfire/Shaders/*.geom
                          Campfire/Shaders/*.vert)

file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})

source_group("Sources"                      FILES ${PROJECT_SOURCES})
source_group("Sources\\Core"                FILES ${SRC_CORE})
source_group("Sources\\Events"              FILES ${SRC_EVENTS})
source_group("Sources\\Platform\\OpenGL"    FILES ${SRC_PLATFORM_OPENGL})
source_group("Sources\\Platform\\Windows"   FILES ${SRC_PLATFORM_WINDOWS})
source_group("Sources\\Renderer"            FILES ${SRC_RENDERER})

source_group("Vendors" FILES ${VENDORS_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES})
target_link_libraries(${PROJECT_NAME} assimp glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
                      BulletDynamics BulletCollision LinearMath)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
