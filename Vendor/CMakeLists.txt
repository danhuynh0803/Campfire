# Third-party vendor libraries

# assimp
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(assimp)
set_target_properties(assimp PROPERTIES FOLDER "Vendor/assimp")
set_target_properties(uninstall PROPERTIES FOLDER "Vendor/assimp")
if(MSVC)
    set_target_properties(zlibstatic PROPERTIES FOLDER "Vendor/assimp")
    set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "Vendor/assimp")
endif()

# bullet3
option(BUILD_BULLET2_DEMOS OFF)
option(BUILD_CPU_DEMOS OFF)
option(BUILD_EXTRAS OFF)
option(BUILD_OPENGL3_DEMOS OFF)
option(BUILD_UNIT_TESTS OFF)
add_subdirectory(bullet3)
set_target_properties(Bullet2FileLoader PROPERTIES FOLDER "Vendor/bullet3/")
set_target_properties(Bullet3Collision PROPERTIES FOLDER "Vendor/bullet3/")
set_target_properties(Bullet3Common PROPERTIES FOLDER "Vendor/bullet3/")
set_target_properties(Bullet3Dynamics PROPERTIES FOLDER "Vendor/bullet3/")
set_target_properties(Bullet3Geometry PROPERTIES FOLDER "Vendor/bullet3/")
set_target_properties(Bullet3OpenCL_clew PROPERTIES FOLDER "Vendor/bullet3/")
set_target_properties(BulletCollision PROPERTIES FOLDER "Vendor/bullet3/")
set_target_properties(BulletDynamics PROPERTIES FOLDER "Vendor/bullet3/")
set_target_properties(BulletInverseDynamics PROPERTIES FOLDER "Vendor/bullet3/")
set_target_properties(BulletSoftBody PROPERTIES FOLDER "Vendor/bullet3/")
set_target_properties(LinearMath PROPERTIES FOLDER "Vendor/bullet3/")

# glad
add_subdirectory(glad)
set_target_properties(glad PROPERTIES FOLDER "Vendor")

# glfw
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(glfw)
set_target_properties(glfw PROPERTIES FOLDER "Vendor")

#target_compile_definitions(
#    glfw
#    PUBLIC
#        -DGLFW_INCLUDE_NONE
#        #-DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\"
#)

add_definitions(
    -DGLFW_INCLUDE_NONE
    #-DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\"
)

# glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "glm/")
#add_subdirectory(glm)
#set_target_properties(glm PROPERTIES FOLDER "Vendor")

# imgui
add_subdirectory(imgui)
set_target_properties(imgui PROPERTIES FOLDER "Vendor")

# spdlog
add_library(spdlog INTERFACE)
target_include_directories(spdlog INTERFACE "spdlog/include/")
#set_target_properties(spdlog PROPERTIES FOLDER "Vendor")

# stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "stb/")
#set_target_properties(stb PROPERTIES FOLDER "Vendor")

# json
add_library(json INTERFACE)
target_include_directories(json INTERFACE "nlohmann/include/")

# entt
add_library(entt INTERFACE)
target_include_directories(entt INTERFACE "entt/include/")

# fmod
add_subdirectory(fmod)
set_target_properties(fmodLib PROPERTIES FOLDER "Vendor")

# freetype
add_subdirectory(freetype-2.10.4)
set_target_properties(freetype PROPERTIES FOLDER "Vendor")

# LUA
add_subdirectory(lua-5.4.0)
set_target_properties(lua PROPERTIES FOLDER "Vendor")

# Tracy
add_library(tracy)
target_include_directories(tracy INTERFACE "tracy")
target_sources(tracy PRIVATE tracy/TracyClient.cpp)
if (MSVC)
    target_compile_options(
        tracy
        PUBLIC
            /D TRACY_ENABLE
    )
    ## For multidll projects
    #add_compile_options(/D TRACY_IMPORTS)
endif()
set_target_properties(tracy PROPERTIES FOLDER "Vendor")

# Vulkan
#project(vulkan)

#font-awesome-5
add_library(font-awesome-5 INTERFACE)
target_include_directories(font-awesome-5 INTERFACE "font-awesome-5/include/")

# RuntimeCompiledCPlusPlus
add_subdirectory(RuntimeCompiledCPlusPlus/Aurora)
set_target_properties(RuntimeCompiler PROPERTIES FOLDER "Vendor")
set_target_properties(RuntimeObjectSystem PROPERTIES FOLDER "Vendor")
