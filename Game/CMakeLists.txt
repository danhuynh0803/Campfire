set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(GAME_SRC
    src/GameLayer.cpp
    src/GameApplication.cpp
)

set(GAME_HEADERS
    include/GameLayer.h
)

add_executable(Game
    ${CMAKE_SOURCE_DIR}/Engine/EntryPoint.h
    ${GAME_SRC}
    ${GAME_HEADERS}
)

#if(MSVC)
#    add_compile_options(/W4)
#else()
#    add_compile_options(-Wall -Wextra -Wpedantic)
#    if(NOT WIN32)
#        set(GLAD_LIBRARIES dl)
#    endif()
#endif()

target_link_libraries(Game
    PRIVATE
        Engine
)

# Output game and all related libs here
#set_target_properties(${PROJECT_NAME} PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
