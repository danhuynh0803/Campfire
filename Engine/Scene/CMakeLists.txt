add_library(Scene)

set_target_properties(Scene PROPERTIES FOLDER "Engine/")

target_include_directories(Scene
    PUBLIC
        "Include"
)

# TODO fix this relation
target_include_directories(Scene
    PRIVATE
        ${CMAKE_SOURCE_DIR}/Vendor/bullet3/src/
        ${CMAKE_SOURCE_DIR}/Vendor/fmod/Windows/core/inc/
        ${CMAKE_SOURCE_DIR}/Vendor/fmod/Windows/studio/inc/
)

set(SCENE_SRC
    Source/Camera.cpp
    Source/CameraController.cpp
    Source/Entity.cpp
    Source/Scene.cpp
    Source/SceneManager.cpp
    Source/Skybox.cpp
)

set(SCENE_HEADERS
    Include/Scene/Camera.h
    Include/Scene/CameraController.h
    Include/Scene/Component.h
    Include/Scene/Entity.h
    Include/Scene/Scene.h
    Include/Scene/SceneManager.h
    Include/Scene/ScriptableEntity.h
    Include/Scene/Skybox.h
)

include_directories(${CMAKE_SOURCE_DIR}/Vendor/entt/)

target_sources(Scene
    PRIVATE
        ${SCENE_SRC}
        ${SCENE_HEADERS}
)

target_link_libraries(Scene
    PUBLIC
        Renderer
        entt
        Audio
        json
        Core
        Physics
        Particles
        spdlog
        Scripting
)
