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

target_include_directories(
    Engine
    PUBLIC
        "include/"
        # Bullet include
        ${CMAKE_SOURCE_DIR}/Vendor/bullet3/src/
)

target_sources(
    Engine
    PRIVATE
        ${PHYSICS_SRC}
        ${PHYSICS_HEADERS}
)
