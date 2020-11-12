cmake_minimum_required(VERSION 3.15)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(Engine)

# for including EntryPoint.h
target_include_directories(Engine PUBLIC "./")

set(VS_SRC_DIR "Source Files")
set(VS_INC_DIR "Header Files")


target_sources(
    Engine
    PUBLIC
        EntryPoint.h
)

target_link_libraries(
    Engine
    PUBLIC
        assimp
        BulletDynamics BulletCollision LinearMath
        entt
        glad
        glfw
        glm
        imgui
        json
        spdlog
        stb
        # TODO
        #freetype
        #fmod
)

#TODO move this to vendor
include_directories(
    ${CMAKE_SOURCE_DIR}/Vendor/bullet3/src/
)

# TODO move to freetype subdir
set(FT_DIR "${CMAKE_SOURCE_DIR}/Vendor/freetype")
if(MSVC)
    target_link_libraries(Engine PUBLIC freetype)
    add_custom_command(
        TARGET Engine
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${FT_DIR}/win64/freetype.dll ${CMAKE_BINARY_DIR}/lib)

    include_directories(
        Engine
        ${FT_DIR}/include/
    )
    link_directories(
        Engine
        ${FT_DIR}/win64/
    )
else()
    # linux FT
endif()

# TODO move to fmod subdir
set(FMOD_DIR "${CMAKE_SOURCE_DIR}/Vendor/fmod")
if(MSVC)
    target_link_libraries(Engine PUBLIC fmod_vc fmodstudio_vc)
    add_custom_command(
        TARGET Engine
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_DIR}/Windows/core/lib/fmod.dll ${FMOD_DIR}/Windows/studio/lib/fmodstudio.dll ${CMAKE_BINARY_DIR}/lib)

    include_directories(
        Engine
        ${FMOD_DIR}/Windows/core/inc/
        ${FMOD_DIR}/Windows/studio/inc/
    )

    link_directories(
        Engine
        ${FMOD_DIR}/Windows/core/lib/
        ${FMOD_DIR}/Windows/studio/lib/
    )
else()
    target_link_libraries(Engine PRIVATE fmod fmodstudio)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_DIR}/Linux/core/lib/x86_64/libfmod.so ${FMOD_DIR}/Linux/studio/lib/x86_64/libfmodstudio.so ${CMAKE_BINARY_DIR}/lib)
endif()

add_subdirectory(Audio)
#add_subdirectory(Command)
add_subdirectory(Core)
add_subdirectory(Events)
add_subdirectory(ImGui)
#add_subdirectory(JobSystem)
add_subdirectory(Particles)
add_subdirectory(Physics)
add_subdirectory(Platform)
add_subdirectory(Renderer)
add_subdirectory(Scene)
add_subdirectory(Scripting)
add_subdirectory(Shaders)
