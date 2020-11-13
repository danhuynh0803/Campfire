set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(GAME_HEADERS
    include/GameLayer.h
)

set(GAME_SRC
    src/GameLayer.cpp
    src/GameApplication.cpp
)

add_executable(
    Game
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

target_include_directories(
    Game
    PUBLIC
        "include"
)

target_link_libraries(
    Game
    PRIVATE
        Engine
)

set(FMOD_DIR "${CMAKE_SOURCE_DIR}/Vendor/fmod")
set(FT_DIR "${CMAKE_SOURCE_DIR}/Vendor/freetype")
if(MSVC)
    add_custom_command(
        TARGET Game
        POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy
                ${FMOD_DIR}/Windows/core/lib/fmod.dll
                ${FMOD_DIR}/Windows/studio/lib/fmodstudio.dll
            ${CMAKE_CURRENT_BINARY_DIR}
    )

    add_custom_command(
        TARGET Game
        POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy
                ${FT_DIR}/win64/freetype.dll
            ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()
