target_include_directories(
    Engine
    PUBLIC
        "include"
)

set(LINUX_HEADERS
    include/Platform/Linux/LinuxFileSystem.h
    include/Platform/Linux/LinuxWindow.h
)

set(LINUX_SRC
    src/LinuxFileSystem.cpp
    src/LinuxWindow.cpp
)

target_sources(
    Engine
    PRIVATE
        ${LINUX_SRC}
        ${LINUX_HEADERS}
)
