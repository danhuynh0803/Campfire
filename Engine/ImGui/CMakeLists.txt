set(IMGUI_HEADERS
    include/ImGui/ImGui.h
    include/ImGui/ImGuiLayer.h
    include/ImGui/ImGuiThemes.h
    include/ImGui/VulkanImGuiLayer.h
    include/ImGui/VulkanImGui.h
)

set(IMGUI_SRC
    src/ImGuiLayer.cpp
    src/VulkanImGuiLayer.cpp
    src/VulkanImGui.cpp
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
