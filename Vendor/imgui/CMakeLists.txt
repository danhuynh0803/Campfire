add_library(imgui STATIC)

set(IMGUI_SRC
    src/imgui.cpp
    src/imgui_demo.cpp
    src/imgui_draw.cpp
    src/imgui_impl_glfw.cpp
    src/imgui_impl_opengl3.cpp
    src/imgui_stdlib.cpp
    src/imgui_widgets.cpp
    src/ImGuizmo.cpp
)

set(IMGUI_HEADERS
    include/imgui/imconfig.h
    include/imgui/imgui.h
    include/imgui/imgui_impl_glfw.h
    include/imgui/imgui_impl_opengl3.h
    include/imgui/imgui_internal.h
    include/imgui/imgui_stdlib.h
    include/imgui/ImGuizmo.h
    include/imgui/imstb_rectpack.h
    include/imgui/imstb_textedit.h
    include/imgui/imstb_truetype.h
)

target_include_directories(imgui PUBLIC "include/imgui/")

target_sources(imgui
    PRIVATE
        ${IMGUI_SRC}
        ${IMGUI_HEADERS}
)

target_link_libraries(imgui
    PUBLIC
        glfw
        glad
)
