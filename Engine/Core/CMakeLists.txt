target_include_directories(
    Engine
    PUBLIC
        "include"
)

set(CORE_HEADERS
    include/Core/Application.h
    include/Core/Base.h
    include/Core/FileSystem.h
    include/Core/Input.h
    include/Core/Layer.h
    include/Core/LayerStack.h
    include/Core/Log.h
    include/Core/MouseCodes.h
    include/Core/Random.h
    include/Core/ResourceManager.h
    include/Core/Time.h
    include/Core/Timer.h
    include/Core/Window.h
)

set(CORE_SRC
    src/Core/Application.cpp
    src/Core/FileSystem.cpp
    src/Core/Input.cpp
    src/Core/Layer.cpp
    src/Core/LayerStack.cpp
    src/Core/Log.cpp
    src/Core/Random.cpp
    src/Core/ResourceManager.cpp
    src/Core/Time.cpp
    src/Core/Window.cpp
)

#MACRO(SOURCE_GROUP_BY_FOLDER target)
#  SET(SOURCE_GROUP_DELIMITER "/")
#  SET(last_dir "")
#  SET(files "")
#  FOREACH(file ${${target}_SRC} ${${target}_HEADERS})
#    file(RELATIVE_PATH relative_file "${PROJECT_SOURCE_DIR}/${target}" ${file})
#    GET_FILENAME_COMPONENT(dir "${relative_file}" PATH)
#    IF (NOT "${dir}" STREQUAL "${last_dir}")
#      IF (files)
#        SOURCE_GROUP("${last_dir}" FILES ${files})
#      ENDIF (files)
#      SET(files "")
#    ENDIF (NOT "${dir}" STREQUAL "${last_dir}")
#    SET(files ${files} ${file})
#    SET(last_dir "${dir}")
#  ENDFOREACH(file)
#  IF (files)
#    SOURCE_GROUP("${last_dir}" FILES ${files})
#  ENDIF (files)
#ENDMACRO(SOURCE_GROUP_BY_FOLDER)

target_sources(Engine
    PRIVATE
        ${CORE_SRC}
        ${CORE_HEADERS}
)

#source_group(
#    "Core"
#    FILES
#        ${CMAKE_CURRENT_SOURCE_DIR}/${CORE_HEADERS}
#        ${CMAKE_CURRENT_SOURCE_DIR}/${CORE_SRC}
#)


