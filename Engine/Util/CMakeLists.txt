target_include_directories(
    Engine
    PUBLIC
        "include"
)

set(UTIL_HEADERS
    include/Util/AABB.h
    include/Util/Ray.h
    include/Util/Utility.h
)

set(UTIL_SRC
    src/Utility.cpp
)

target_sources(Engine
    PRIVATE
        ${UTIL_SRC}
        ${UTIL_HEADERS}
)
