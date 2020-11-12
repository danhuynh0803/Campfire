target_include_directories(
    Engine
    PUBLIC
        "include/"
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

target_sources(
    Engine
    PRIVATE
        ${AUDIO_SRC}
        ${AUDIO_HEADERS}
)
