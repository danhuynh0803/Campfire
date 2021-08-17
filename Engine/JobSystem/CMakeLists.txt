set(JOBSYSTEM_HEADERS
    include/JobSystem/InterruptibleThread.h
    include/JobSystem/Job.h
    include/JobSystem/JobStealingQueue.h
    include/JobSystem/JobSystem.h
    include/JobSystem/JobThreadsJoiner.h
    include/JobSystem/ThreadSafeQueue.h
)

set(OBSYSTEM_SRC
    src/InterruptibleThread.cpp
    src/JobSystem.cpp
)

target_include_directories(
    Engine
    PUBLIC
        "include/"
)

target_sources(
    Engine
    PRIVATE
        ${JOBSYSTEM_SRC}
        ${JOBSYSTEM_HEADERS}
)
