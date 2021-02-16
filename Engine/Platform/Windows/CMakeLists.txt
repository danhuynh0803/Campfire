target_include_directories(
    Engine
    PUBLIC
        "include"
)

set(WINDOWS_HEADERS
    include/Platform/Windows/WindowsFileSystem.h
    include/Platform/Windows/WindowsWindow.h
    include/Platform/Windows/WindowsProcessorInfo.h
)

set(WINDOWS_SRC
    src/Windows/WindowsFileSystem.cpp
    src/Windows/WindowsWindow.cpp
    src/Windows/WindowsProcessorInfo.cpp
)

target_sources(
    Engine
    PRIVATE
        ${WINDOWS_SRC}
        ${WINDOWS_HEADERS}
)
