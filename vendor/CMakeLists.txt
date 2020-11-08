set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(json)
include_directories(
    nlohmann_json/
)

add_subdirectory(spdlog)
