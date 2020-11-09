cmake_minimum_required(VERSION 3.15)

set(EVENTS_HEADERS
    Include/Events/ApplicationEvent.h
    Include/Events/Event.h
    Include/Events/KeyEvent.h
    Include/Events/MouseEvent.h
)

add_library(Events)

set_target_properties(Events PROPERTIES FOLDER "Engine")
set_target_properties(Events PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Events PUBLIC "Include/")

# Create a empty file just to tell VS to make the lib
file(WRITE null.cpp "")

target_sources(Events
    PRIVATE
        ${EVENTS_HEADERS}
        null.cpp
)

target_link_libraries(Events)
