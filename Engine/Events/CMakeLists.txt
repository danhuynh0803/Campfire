#TODO should prolly just move events into core
set(EVENTS_HEADERS
    include/Events/ApplicationEvent.h
    include/Events/Event.h
    include/Events/KeyEvent.h
    include/Events/MouseEvent.h
    include/Events/LuaEvent.h
    include/Events/LuaManagerEvent.h
    include/Events/EventManager.h
)

target_include_directories(
    Engine
    PUBLIC
        "include"
)

target_sources(
    Engine
    PRIVATE
        ${EVENTS_HEADERS}
)
