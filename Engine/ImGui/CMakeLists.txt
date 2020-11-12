set(IMGUI_HEADERS
    include/ImGui/ImGui.h
    include/ImGui/ImGuiLayer.h
    include/ImGui/ImGuiThemes.h
)

set(IMGUI_SRC
    src/ImGuiLayer.cpp
)

target_include_directories(
    Engine
    PUBLIC
        "include/"
)

target_sources(
    Engine
    PRIVATE
        ${IMGUI_SRC}
        ${IMGUI_HEADERS}
)
