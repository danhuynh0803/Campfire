set(VK_HEADERS
    include/VulkanLayer.h
)

set(VK_SRC
    src/VulkanLayer.cpp
    src/VulkanApplication.cpp
)

add_executable(
    VkApp
    ${CMAKE_SOURCE_DIR}/Engine/EntryPoint.h
    ${VK_SRC}
    ${VK_HEADERS}
)

#if(MSVC)
#    add_compile_options(/W4)
#else()
#    add_compile_options(-Wall -Wextra -Wpedantic)
#    if(NOT WIN32)
#        set(GLAD_LIBRARIES dl)
#    endif()
#endif()

target_include_directories(
    VkApp
    PUBLIC
        "include"
)

target_link_libraries(
    VkApp
    PRIVATE
        Engine
)

set(FMOD_DIR "${CMAKE_SOURCE_DIR}/Vendor/fmod")
set(FT_DIR "${CMAKE_SOURCE_DIR}/Vendor/freetype")
if(MSVC)
    add_custom_command(
        TARGET VkApp
        POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy
                ${FMOD_DIR}/Windows/core/lib/fmod.dll
                ${FMOD_DIR}/Windows/studio/lib/fmodstudio.dll
            ${CMAKE_CURRENT_BINARY_DIR}
    )

    add_custom_command(
        TARGET VkApp
        POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy
                ${FT_DIR}/win64/freetype.dll
            ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()
