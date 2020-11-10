# Third-party vendor libraries

# assimp
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(assimp)
set_target_properties(assimp PROPERTIES FOLDER "Vendor/assimp")
set_target_properties(zlibstatic PROPERTIES FOLDER "Vendor/assimp")
set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "Vendor/assimp")
set_target_properties(uninstall PROPERTIES FOLDER "Vendor/assimp")

# bullet3
#option(BUILD_BULLET2_DEMOS OFF)
#option(BUILD_CPU_DEMOS OFF)
#option(BUILD_EXTRAS OFF)
#option(BUILD_OPENGL3_DEMOS OFF)
#option(BUILD_UNIT_TESTS OFF)
#add_subdirectory(bullet3)
#set_target_properties(bullet3 PROPERTIES FOLDER "Vendor")

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
add_library(glm INTERFACE)
target_include_directories(glm
    INTERFACE
        "glm/"
)
#add_subdirectory(glm)
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
