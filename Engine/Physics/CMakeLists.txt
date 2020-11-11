add_library(Physics)

set_target_properties(Physics PROPERTIES FOLDER "Engine/")

target_include_directories(Physics
    PUBLIC
        "include/"
        ${CMAKE_SOURCE_DIR}/Vendor/bullet3/src/
)

set(PHYSICS_HEADERS
    include/Physics/BulletDebugDrawer.h
    include/Physics/Collider.h
    include/Physics/PhysicsManager.h
    include/Physics/Rigidbody.h
    include/Physics/Trigger.h
)

set(PHYSICS_SRC
    src/Collider.cpp
    src/PhysicsManager.cpp
    src/Rigidbody.cpp
    src/Trigger.cpp
)

target_sources(Physics
    PRIVATE
        ${PHYSICS_SRC}
        ${PHYSICS_HEADERS}
)


target_link_libraries(Physics
    PUBLIC
        BulletDynamics
        BulletCollision
        LinearMath
        Core
        Scene
        glm
)
