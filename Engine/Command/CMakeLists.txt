target_include_directories(
    Engine
    PUBLIC
        "include"
)

set(COMMAND_HEADERS
    include/Command/Command.h
    include/Command/CommandManager.h
)

set(COMMAND_SRC
    src/CommandManager.cpp
)

target_sources(
    Engine
    PRIVATE
        ${COMMAND_SRC}
        ${COMMAND_HEADERS}
)
