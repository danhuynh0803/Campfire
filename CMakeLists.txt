cmake_minimum_required(VERSION 3.20)

# Explicitly disable vcpkg integration
# We use git submodules for dependency management
set(VCPKG_MANIFEST_MODE OFF CACHE BOOL "Disable vcpkg manifest mode" FORCE)
set(VCPKG_TARGET_TRIPLET "" CACHE STRING "Disable vcpkg target triplet" FORCE)

# Set all libs to be static
option(BUILD_SHARED_LIBS OFF)

# Use C++20 for main project targets
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable compiler-specific extensions for better portability
# This uses -std=c++20 instead of -std=gnu++20 on GCC/Clang
# and /std:c++20 instead of language extensions on MSVC
if(MSVC)
    set(CMAKE_CXX_EXTENSIONS OFF)
else()
    # GCC/Clang: Allow GNU extensions for POSIX compatibility
    set(CMAKE_CXX_EXTENSIONS ON)
endif()

project(Campfire
    #    VERSION 0.1
    #    DESCRIPTION "Game Engine"
    #    LANGUAGES CXX
)

#if building for IDE, enable folders
if(MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    
    # Required compiler options for modern dependencies
    # IMPORTANT: Must be set AFTER project() command
    string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus /wd4251 /wd4275 /WX- /W2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "MSVC C++ flags" FORCE)
    
    string(APPEND CMAKE_CXX_FLAGS_RELEASE " /MT")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "MSVC C++ Release flags" FORCE)
    
    string(APPEND CMAKE_CXX_FLAGS_DEBUG " /MTd")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "MSVC C++ Debug flags" FORCE)
    
    # Explanation:
    # /Zc:__cplusplus - Enable correct __cplusplus macro value  
    # /wd4251       - Disable DLL interface warning
    # /wd4275       - Disable DLL base class warning
    # /WX-          - Do NOT treat warnings as errors (important for vendor libs)
    # /W2           - Warning level 2 (less strict, better for vendor libs)
    # 
    # Note: UTF-8 support is handled per-target:
    #   - assimp: Adds /source-charset:utf-8 in its CMakeLists.txt
    #   - RuntimeCompiledCPlusPlus: Adds /utf-8 for its targets
    #   - Engine/Editor: Will add /utf-8 in their respective CMakeLists.txt
endif()

file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if (EXISTS "${LOC_PATH}")
  message(FATAL_ERROR "build must be out of source")
endif()

add_subdirectory(Engine)
add_subdirectory(Vendor)
add_subdirectory(Editor)
#add_subdirectory(Game)
