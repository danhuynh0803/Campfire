set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(EDITOR_HEADERS
    include/Widgets/HierarchyWidget.h
    include/Widgets/InspectorWidget.h
    #include/Widgets/ImGuiLog.h
    #include/Widgets/LogWidget.h
    include/Widgets/TransformWidget.h
    #include/Widgets/AssetBrowser.h
    #include/EditorLayer.h
    include/RayTracingLayer.h
    include/VulkanLayer.h
)

set(EDITOR_SRC
    src/Widgets/HierarchyWidget.cpp
    src/Widgets/InspectorWidget.cpp
    #src/Widgets/LogWidget.cpp
    src/Widgets/TransformWidget.cpp
    #src/Widgets/AssetBrowser.cpp
    #src/EditorLayer.cpp
    src/RayTracingLayer.cpp
    src/VulkanLayer.cpp
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

target_link_libraries(
    Editor
    PRIVATE
        Engine
        #font-awesome-5
)

set(FMOD_DIR "${CMAKE_SOURCE_DIR}/Vendor/fmod")
set(FT_DIR "${CMAKE_SOURCE_DIR}/Vendor/freetype-2.10.4")
if(MSVC)
    add_custom_command(
        TARGET Editor
        POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy
                ${FMOD_DIR}/Windows/core/lib/fmod.dll
                ${FMOD_DIR}/Windows/studio/lib/fmodstudio.dll
            ${CMAKE_CURRENT_BINARY_DIR}
    )

    #add_custom_command(
        #TARGET Editor
        #POST_BUILD COMMAND
            #${CMAKE_COMMAND} -E copy
                #${FT_DIR}/win64/freetype.dll
            #${CMAKE_CURRENT_BINARY_DIR}
    #)
else()
    link_directories(
        ${FMOD_DIR}/Linux/core/lib/x86_64/
        ${FMOD_DIR}/Linux/studio/lib/x86_64/
    )

    add_custom_command(
        TARGET Editor
        POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy
                ${FMOD_DIR}/Linux/core/lib/x86_64/libfmod.so
                ${FMOD_DIR}/Linux/studio/lib/x86_64/libfmodstudio.so
            ${CMAKE_CURRENT_BINARY_DIR}
    )

#    add_custom_command(
#        TARGET Editor
#        POST_BUILD COMMAND
#            ${CMAKE_COMMAND} -E copy
#                ${FT_DIR}/win64/freetype.dll
#            ${CMAKE_CURRENT_BINARY_DIR}
#    )
endif()
