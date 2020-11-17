target_include_directories(
    Engine
    PUBLIC
        "include"
)

set(SCRIPTING_HEADERS
    include/Scripting/ArenaAllocator.h
    include/Scripting/CameraController.h
    include/Scripting/PlayerController.h
    include/Scripting/Script.h
    include/Scripting/LuaScript.h
    include/Scripting/ScriptEngine.h
    include/Scripting/Lua/LuaTransform.h
    include/Scripting/Lua/LuaTag.h
)

set(SCRIPTING_SRC
    src/Script.cpp
    src/ScriptEngine.cpp
)

target_sources(
    Engine
    PRIVATE
        ${SCRIPTING_SRC}
        ${SCRIPTING_HEADERS}
)
