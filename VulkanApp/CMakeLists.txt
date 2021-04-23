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

#TODO put this post build commands in one spot and just find the current application that is being run?
set(FMOD_DIR "${CMAKE_SOURCE_DIR}/Vendor/fmod")
set(FT_DIR "${CMAKE_SOURCE_DIR}/Vendor/freetype-2.10.4")
if(MSVC)
    add_custom_command(
        TARGET VkApp
        POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy
                ${FMOD_DIR}/Windows/core/lib/fmod.dll
                ${FMOD_DIR}/Windows/studio/lib/fmodstudio.dll
            ${CMAKE_CURRENT_BINARY_DIR}
    )

    #add_custom_command(
        #TARGET Editor
        #POST_BUILD COMMAND
            #${CMAKE_COMMAND} -E copy
                #${FT_DIR}/win64/freetype.dll
            #${CMAKE_CURRENT_BINARY_DIR}
    #)
else()
    link_directories(
        ${FMOD_DIR}/Linux/core/lib/x86_64/
        ${FMOD_DIR}/Linux/studio/lib/x86_64/
    )

    add_custom_command(
        TARGET VkApp
        POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy
                ${FMOD_DIR}/Linux/core/lib/x86_64/libfmod.so
                ${FMOD_DIR}/Linux/studio/lib/x86_64/libfmodstudio.so
            ${CMAKE_CURRENT_BINARY_DIR}
    )

#    add_custom_command(
#        TARGET Editor
#        POST_BUILD COMMAND
#            ${CMAKE_COMMAND} -E copy
#                ${FT_DIR}/win64/freetype.dll
#            ${CMAKE_CURRENT_BINARY_DIR}
#    )
endif()
