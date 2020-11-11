add_library(Particles)

set_target_properties(Particles PROPERTIES FOLDER "Engine/")

target_include_directories(Particles PUBLIC "include/")

set(PARTICLES_HEADERS
    include/Particles/Particle.h
    include/Particles/ParticleSystem.h
)

set(PARTICLES_SRC
    src/ParticleSystem.cpp
)

target_sources(Particles
    PRIVATE
        ${PARTICLES_SRC}
        ${PARTICLES_HEADERS}
)


target_link_libraries(Particles
    PRIVATE
        Core
        glm
)
