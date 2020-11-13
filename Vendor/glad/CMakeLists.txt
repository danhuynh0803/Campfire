add_library(glad)

target_include_directories(glad PUBLIC "include/")

target_sources(
    glad
    PRIVATE
        src/glad.c
)

find_package(OpenGL REQUIRED)

set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(
    glad
    PUBLIC
        ${CMAKE_DL_LIBS}
)
