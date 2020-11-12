file(WRITE null.cpp "")
add_library(freetype2 null.cpp)

target_include_directories(
    freetype2
    PUBLIC
        "include"
)

if(MSVC)
    target_link_libraries(
        freetype2
        PRIVATE
            freetype
    )

    target_link_directories(
        freetype2
        PUBLIC
            "win64/"
    )

#    add_custom_command(
#        TARGET Editor
#        TARGET Game
#        POST_BUILD COMMAND
#            ${CMAKE_COMMAND} -E copy
#                ${FT_DIR}/win64/freetype.dll
#            ${CMAKE_BINARY_DIR}/lib
#    )

#    install(
#        TARGETS
#        DESTINATION
#    )
else()


endif()
