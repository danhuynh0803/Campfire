set(GAME_HEADERS
    include/GameLayer.h
    include/RCCpp.h
    include/RCCppEntry.h
    include/RCCppSystemTable.h
    include/RCCppEngine
    include/StdioLogSystem.h
    include/TestX.h
)

set(GAME_SRC
    src/GameLayer.cpp
    src/RCCpp.cpp
    src/GameApplication.cpp
    src/RCCppEntry.cpp
    src/StdioLogSystem.cpp
    src/TestX.cpp
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

#    add_custom_command(
#        TARGET Game
#        POST_BUILD COMMAND
#            ${CMAKE_COMMAND} -E copy
#                ${FT_DIR}/win64/freetype.dll
#            ${CMAKE_CURRENT_BINARY_DIR}
#    )
else()
    link_directories(
        ${FMOD_DIR}/Linux/core/lib/x86_64/
        ${FMOD_DIR}/Linux/studio/lib/x86_64/
    )

    add_custom_command(
        TARGET Game
        POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy
                ${FMOD_DIR}/Linux/core/lib/x86_64/libfmod.so
                ${FMOD_DIR}/Linux/studio/lib/x86_64/libfmodstudio.so
            ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()
