add_library(ImGui STATIC)

set(IMGUI_SRC
    Source/ImGuiLayer.cpp
)

set(IMGUI_HEADERS
    Include/ImGui/ImGui.h
    Include/ImGui/ImGuiLayer.h
    Include/ImGui/ImGuiThemes.h
)

set_target_properties(ImGui PROPERTIES FOLDER "Engine")

target_include_directories(ImGui PUBLIC "Include/")

target_sources(ImGui
    PRIVATE
        ${IMGUI_SRC}
        ${IMGUI_HEADERS}
)

target_link_libraries(ImGui
    PUBLIC
        imgui
    PRIVATE
        Core
        glfw
        glad
)
