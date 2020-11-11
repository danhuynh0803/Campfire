add_library(MyImGui)

set(IMGUI_SRC
    Source/ImGuiLayer.cpp
)

set(IMGUI_HEADERS
    Include/ImGui/ImGui.h
    Include/ImGui/ImGuiLayer.h
    Include/ImGui/ImGuiThemes.h
)

set_target_properties(MyImGui PROPERTIES FOLDER "Engine")

target_include_directories(MyImGui PUBLIC "Include/")

target_sources(MyImGui
    PRIVATE
        ${IMGUI_SRC}
        ${IMGUI_HEADERS}
)

target_link_libraries(MyImGui
    PUBLIC
        imgui
    PRIVATE
        Core
        glfw
        glad
)
