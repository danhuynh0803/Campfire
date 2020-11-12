file(WRITE null.cpp "")
add_library(fmod null.cpp)

if(MSVC)
    target_link_libraries(
        fmod
        PRIVATE
            fmod_vc
            fmodstudio_vc
    )

    target_include_directories(
        fmod
        PUBLIC
            "Windows/core/inc"
            "Windows/studio/inc"
    )

    target_link_directories(
        fmod
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
    #    target_link_libraries(Engine PRIVATE fmod fmodstudio)
    #    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_DIR}/Linux/core/lib/x86_64/libfmod.so ${FMOD_DIR}/Linux/studio/lib/x86_64/libfmodstudio.so ${CMAKE_BINARY_DIR}/lib)
endif()
