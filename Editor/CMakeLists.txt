set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(EDITOR_HEADERS
    include/Widgets/HierarchyWidget.h
    include/Widgets/InspectorWidget.h
    include/Widgets/LogWidget.h
    include/Widgets/TransformWidget.h
    include/EditorLayer.h
)

set(EDITOR_SRC
    src/Widgets/HierarchyWidget.cpp
    src/Widgets/InspectorWidget.cpp
    #src/Widgets/LogWidget.cpp
    src/Widgets/TransformWidget.cpp
    src/EditorLayer.cpp
    src/EditorApplication.cpp
)

#if(MSVC)
#    add_compile_options(/W4)
#else()
#    add_compile_options(-Wall -Wextra -Wpedantic)
#    if(NOT WIN32)
#        set(GLAD_LIBRARIES dl)
#    endif()
#endif()

set(FT_DIR "${CMAKE_SOURCE_DIR}/Vendor/freetype")
if(MSVC)
    include_directories(
        Editor
        ${FT_DIR}/include/
    )
    link_directories(
        Editor
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
    Editor
    ${CMAKE_SOURCE_DIR}/Engine/EntryPoint.h
    ${EDITOR_SRC}
    ${EDITOR_HEADERS}
)

target_include_directories(
    Editor
    PUBLIC
        "include"
)

target_include_directories(
    Editor
    PUBLIC
        "include"
    PRIVATE
        ${CMAKE_SOURCE_DIR}/Vendor/bullet3/src/
)

target_link_libraries(
    Editor
    PRIVATE
        Engine
        fmod_vc
        fmodstudio_vc
        freetype
)

if(MSVC)
    add_custom_command(
        TARGET Editor
        POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy
                ${FMOD_DIR}/Windows/core/lib/fmod.dll
                ${FMOD_DIR}/Windows/studio/lib/fmodstudio.dll
            ${CMAKE_BINARY_DIR}/lib
    )

    add_custom_command(
        TARGET Editor
        POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy
                ${FT_DIR}/win64/freetype.dll
            ${CMAKE_BINARY_DIR}/lib
    )

else()

endif()

set_target_properties(Editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Editor)
