set(PARTICLES_HEADERS
    include/Particles/Particle.h
    include/Particles/ParticleSystem.h
)

set(PARTICLES_SRC
    src/ParticleSystem.cpp
)

target_include_directories(
    Engine
    PUBLIC
        "include/"
)

target_sources(
    Engine
    PRIVATE
        ${PARTICLES_SRC}
        ${PARTICLES_HEADERS}
)
