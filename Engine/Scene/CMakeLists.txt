target_include_directories(
    Engine
    PUBLIC
        "include"
)

set(SCENE_HEADERS
    include/Scene/Camera.h
    include/Scene/CameraController.h
    include/Scene/Component.h
    include/Scene/Entity.h
    include/Scene/Scene.h
    include/Scene/SceneManager.h
    include/Scene/ScriptableEntity.h
    include/Scene/Skybox.h
)

set(SCENE_SRC
    src/Camera.cpp
    src/CameraController.cpp
    src/Entity.cpp
    src/Scene.cpp
    src/SceneManager.cpp
    src/Skybox.cpp
)

target_sources(
    Engine
    PRIVATE
        ${SCENE_SRC}
        ${SCENE_HEADERS}
)
