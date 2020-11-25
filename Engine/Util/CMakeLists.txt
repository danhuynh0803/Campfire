target_include_directories(
    Engine
    PUBLIC
        "include"
)

set(UTIL_HEADERS
    include/Util/AABB.h
    include/Util/Ray.h
)

set(UTIL_SRC
)

target_sources(Engine
    PRIVATE
        ${UTIL_HEADERS}
)
