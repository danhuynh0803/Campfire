# Third-party vendor libraries

# glad
add_subdirectory(glad)


# glfw
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(glfw)
#include_directories(glfw/include/)


# glm
add_subdirectory(glm)
#include_directories(glm)


# spdlog
add_subdirectory(spdlog)


# stb
include_directories(stb)
