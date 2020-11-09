# Third-party vendor libraries

# glad
add_subdirectory(glad)
set_target_properties(glad PROPERTIES FOLDER "Vendor")

# glfw
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(glfw)
set_target_properties(glfw PROPERTIES FOLDER "Vendor")

# glm
add_subdirectory(glm)
#include_directories(glm)
#set_target_properties(glm PROPERTIES FOLDER "Vendor/glm")

# imgui
add_subdirectory(imgui)
set_target_properties(imgui PROPERTIES FOLDER "Vendor")

# spdlog
add_subdirectory(spdlog)
set_target_properties(spdlog PROPERTIES FOLDER "Vendor")

# stb
include_directories(stb)
#set_target_properties(stb PROPERTIES FOLDER "Vendor/stb")
