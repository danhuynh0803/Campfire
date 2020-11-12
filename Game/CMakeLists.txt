set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(GAME_HEADERS
    include/GameLayer.h
)

set(GAME_SRC
    src/GameLayer.cpp
    src/GameApplication.cpp
)

set(FT_DIR "${CMAKE_SOURCE_DIR}/Vendor/freetype")
if(MSVC)
    include_directories(
        ${FT_DIR}/include/
    )
    link_directories(
        ${FT_DIR}/win64/
    )
else()
    # linux FT
endif()


set(FMOD_DIR "${CMAKE_SOURCE_DIR}/Vendor/fmod")
message("${FMOD_DIR}")
if(MSVC)
    find_library(FMOD_LIB fmod_vc)
    find_library(FMOD_STUDIO_LIB fmodstudio_vc)

    include_directories(
        ${FMOD_DIR}/Windows/core/inc/
        ${FMOD_DIR}/Windows/studio/inc/
    )

    link_directories(
        ${FMOD_DIR}/Windows/core/lib/
        ${FMOD_DIR}/Windows/studio/lib/
    )
else()
    #target_link_libraries(Editor fmod fmodstudio)
    #add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_DIR}/Linux/core/lib/x86_64/libfmod.so ${FMOD_DIR}/Linux/studio/lib/x86_64/libfmodstudio.so ${CMAKE_BINARY_DIR}/lib)
endif()


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
        fmod_vc
        fmodstudio_vc
        freetype
)

if(MSVC)
    add_custom_command(
        TARGET Game
        POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy
                ${FMOD_DIR}/Windows/core/lib/fmod.dll
                ${FMOD_DIR}/Windows/studio/lib/fmodstudio.dll
            ${CMAKE_BINARY_DIR}/lib
    )

    add_custom_command(
        TARGET Game
        POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy
                ${FT_DIR}/win64/freetype.dll
            ${CMAKE_BINARY_DIR}/lib
    )

else()

endif()

set_target_properties(Game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Game)

#set_target_properties(Game PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Game)

# Output game and all related libs here
#set_target_properties(${PROJECT_NAME} PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
