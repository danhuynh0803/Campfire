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
        freetype2
        fmod
)

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
