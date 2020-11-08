cmake_minimum_required(VERSION 3.15)

add_library(Events)

target_include_directories(Events PUBLIC "Include/")

# dummy file since we can't create a lib without some source
file(WRITE Source/Events/null.cpp "")

target_sources(Events
    PRIVATE
        Source/Events/null.cpp
)

target_link_libraries(Events)
