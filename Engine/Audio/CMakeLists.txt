add_library(Audio)

set_target_properties(Audio PROPERTIES FOLDER "Engine/")

target_include_directories(Audio
    PUBLIC
        "include/"
)

# TODO add cmake to FMOD
target_include_directories(Audio
    PUBLIC
        ${CMAKE_SOURCE_DIR}/Vendor/fmod/Windows/core/inc/
        ${CMAKE_SOURCE_DIR}/Vendor/fmod/Windows/studio/inc/
)

set(AUDIO_HEADERS
    include/Audio/AudioClip.h
    include/Audio/AudioSource.h
    include/Audio/AudioSystem.h
    include/Audio/FmodImpl.h
)

set(AUDIO_SRC
    src/AudioSource.cpp
    src/AudioSystem.cpp
    src/FmodImpl.cpp
)

target_sources(Audio
    PRIVATE
        ${AUDIO_SRC}
        ${AUDIO_HEADERS}
)

target_link_libraries(Audio
    PRIVATE
    #fmod TODO
        Core
)
