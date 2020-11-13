cmake_minimum_required(VERSION 3.18)

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
        freetype2
        fmod
        glm
        entt
        imgui
        spdlog
        json
    PRIVATE
        BulletDynamics BulletCollision LinearMath
        stb
        lua
        glad
        glfw
)

MACRO(SOURCE_GROUP_BY_FOLDER target)
  SET(SOURCE_GROUP_DELIMITER "/")
  SET(last_dir "")
  SET(files "")
  FOREACH(file ${${target}_SRC} ${${target}_HEADERS})
    file(RELATIVE_PATH relative_file "${PROJECT_SOURCE_DIR}/${target}" ${file})
    GET_FILENAME_COMPONENT(dir "${relative_file}" PATH)
    IF (NOT "${dir}" STREQUAL "${last_dir}")
      IF (files)
        SOURCE_GROUP("${last_dir}" FILES ${files})
      ENDIF (files)
      SET(files "")
    ENDIF (NOT "${dir}" STREQUAL "${last_dir}")
    SET(files ${files} ${file})
    SET(last_dir "${dir}")
  ENDFOREACH(file)
  IF (files)
    SOURCE_GROUP("${last_dir}" FILES ${files})
  ENDIF (files)
ENDMACRO(SOURCE_GROUP_BY_FOLDER)

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
