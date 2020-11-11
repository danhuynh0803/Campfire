add_library(Scripting)

set_target_properties(Scripting PROPERTIES FOLDER "Engine/")

target_include_directories(Scripting
    PUBLIC
        "include/"
    PRIVATE
        ${CMAKE_SOURCE_DIR}/Vendor/bullet3/src/
)

set(SCRIPTING_HEADERS
    include/Scripting/ArenaAllocator.h
    include/Scripting/CameraController.h
    include/Scripting/PlayerController.h
    include/Scripting/Script.h
    include/Scripting/ScriptEngine.h
)

set(SCRIPTING_SRC
    src/ScriptEngine.cpp
)

target_sources(Scripting
    PRIVATE
        ${SCRIPTING_SRC}
        ${SCRIPTING_HEADERS}
)


target_link_libraries(Scripting
    PUBLIC
        Core
        Scene
        glm
)
