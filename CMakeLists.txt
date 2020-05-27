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

SET(MACOSX_DEPLOYMENT_TARGET ${DARWIN_MAJOR_SDK_VERSION}.${DARWIN_MINOR_SDK_VERSION})
SET(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")
MESSAGE("Setting MACOSX_DEPLOYMENT_TARGET to '${MACOSX_DEPLOYMENT_TARGET}'.")

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
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
                    Campfire/Vendor/rapidjson/include)

file(GLOB VENDORS_SOURCES Campfire/Vendor/glad/src/glad.c)

file(GLOB PROJECT_HEADERS Campfire/Headers/*.hpp)

file(GLOB PROJECT_SOURCES Campfire/Sources/*.cpp
                          Campfire/Sources/imgui/*.cpp)

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
source_group("Sources" FILES ${PROJECT_SOURCES})
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
