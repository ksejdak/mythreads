# output settings
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_HOME_DIRECTORY}/bin)

# source files
set(SOURCES
    thread.c
    thread_structures.h
    scheduler.c
    mutex.c
)

# executable target
add_executable(${APP_NAME} main.c ${SOURCES})


