cmake_minimum_required(VERSION 3.15)

target_include_directories(
    Engine
    PUBLIC
        "include"
)

set(WINDOWS_HEADERS
    include/Platform/Windows/WindowsFileSystem.h
    include/Platform/Windows/WindowsWindow.h
)

set(WINDOWS_SRC
    src/Windows/WindowsFileSystem.cpp
    src/Windows/WindowsWindow.cpp
)

target_sources(
    Engine
    PRIVATE
        ${WINDOWS_SRC}
        ${WINDOWS_HEADERS}
)
