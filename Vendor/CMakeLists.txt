# Third-party vendor libraries

# assimp
add_subdirectory(assimp)
set_target_properties(assimp PROPERTIES FOLDER "Vendor")

# bullet3
add_subdirectory(bullet3)
set_target_properties(bullet3 PROPERTIES FOLDER "Vendor")

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
#set_target_properties(glm PROPERTIES FOLDER "Vendor")

# imgui
add_subdirectory(imgui)
set_target_properties(imgui PROPERTIES FOLDER "Vendor")

# spdlog
add_subdirectory(spdlog)
set_target_properties(spdlog PROPERTIES FOLDER "Vendor")

# stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "stb/")
#set_target_properties(stb PROPERTIES FOLDER "Vendor")
