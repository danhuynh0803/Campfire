file(WRITE null.cpp "")
add_library(fmodLib null.cpp)

if(MSVC)
    target_link_libraries(
        fmodLib
        PRIVATE
            fmod_vc
            fmodstudio_vc
    )

    target_include_directories(
        fmodLib
        PUBLIC
            "Windows/core/inc"
            "Windows/studio/inc"
    )

    target_link_directories(
        fmodLib
        PUBLIC
            "Windows/core/lib/"
            "Windows/studio/lib/"
    )

#    add_custom_command(
#        #TARGET Engine
#        TARGET Editor
#        TARGET Game
#        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_DIR}/Windows/core/lib/fmod.dll ${FMOD_DIR}/Windows/studio/lib/fmodstudio.dll ${CMAKE_BINARY_DIR}/lib
#    )

else()
    target_link_libraries(
        fmodLib
        PRIVATE
            fmod
            fmodstudio
    )

    target_include_directories(
        fmodLib
        PUBLIC
            "Linux/core/inc"
            "Linux/studio/inc"
    )

    target_link_directories(
        fmodLib
        PUBLIC
            "Linux/core/lib/"
            "Linux/studio/lib/"
    )
endif()
