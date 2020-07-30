cmake_minimum_required(VERSION 3.7)

project( LuaLib )

# The recommended way to collect sources in variable by explicitly specifying the source files

file(GLOB SRC_LUA src/*.h src/*.c src/*.hpp)
  
add_library( LuaLib ${SRC_LUA} )

target_include_directories ( LuaLib PUBLIC "${PROJECT_SOURCE_DIR}/src")
