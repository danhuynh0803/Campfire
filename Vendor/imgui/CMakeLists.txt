add_library(imgui STATIC)

set(IMGUI_SRC
    Source/imgui.cpp
    Source/imgui_demo.cpp
    Source/imgui_draw.cpp
    Source/imgui_impl_glfw.cpp
    Source/imgui_impl_opengl3.cpp
    Source/imgui_stdlib.cpp
    Source/imgui_widgets.cpp
    Source/ImGuizmo.cpp
)

set(IMGUI_HEADERS
    Include/imgui/imconfig.h
    Include/imgui/imgui.h
    Include/imgui/imgui_impl_glfw.h
    Include/imgui/imgui_impl_opengl3.h
    Include/imgui/imgui_internal.h
    Include/imgui/imgui_stdlib.h
    Include/imgui/ImGuizmo.h
    Include/imgui/imstb_rectpack.h
    Include/imgui/imstb_textedit.h
    Include/imgui/imstb_truetype.h
)

target_include_directories(imgui PUBLIC "Include/")

target_sources(imgui
    PRIVATE
        ${IMGUI_SRC}
        ${IMGUI_HEADERS}
)
