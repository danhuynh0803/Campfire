set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(EDITOR_SRC
    #Source/Widgets/HierarchyWidget.cpp
    #Source/Widgets/InspectorWidget.cpp
    #Source/Widgets/LogWidget.cpp
    #Source/Widgets/TransformWidget.cpp
    #Source/EditorLayer.cpp
    Source/EditorApplication.cpp
)

set(EDITOR_HEADERS
    #Include/Widgets/HierarchyWidget.h
    #Include/Widgets/InspectorWidget.h
    #Include/Widgets/LogWidget.h
    #Include/Widgets/TransformWidget.h
    #Include/EditorLayer.h
)

#add_library(Editor)
#
#target_include_directories(Editor PUBLIC "Include/")
#
#target_sources(Editor
#    PRIVATE
#        ${EDITOR_SRC}
#        ${EDITOR_HEADERS}
#)
#
#if(MSVC)
#    add_compile_options(/W4)
#else()
#    add_compile_options(-Wall -Wextra -Wpedantic)
#    if(NOT WIN32)
#        set(GLAD_LIBRARIES dl)
#    endif()
#endif()

add_executable(Editor
    ${CMAKE_SOURCE_DIR}/Engine/EntryPoint.h
    ${EDITOR_SRC}
    ${EDITOR_HEADERS}
)

target_link_libraries(Editor
    PUBLIC
        glfw
    PRIVATE
        Engine
)


#set_target_properties(${PROJECT_NAME} PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
