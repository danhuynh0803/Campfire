add_library(glad STATIC)

target_include_directories(glad PUBLIC "Include/")

target_sources(glad PRIVATE Source/glad.c)

find_package(OpenGL REQUIRED)

set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(glad
    PUBLIC
        ${CMAKE_DL_LIBS}
)
