# output settings
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_HOME_DIRECTORY}/bin)

# source files
set(SOURCES
    ../include/thread.h
    ../include/scheduler.h
    ../include/mutex.h
    thread/thread_impl.h
    thread/thread.c
    scheduler/scheduler_impl.h
    scheduler/scheduler.c
    scheduler/roundrobin_policy.c
    mutex/mutex.c
    utils/queue.c
)

# executable target
add_executable(${APP_NAME} main.c ${SOURCES})


