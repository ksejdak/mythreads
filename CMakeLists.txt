# required cmake version
cmake_minimum_required(VERSION 2.8.12)

# project name
set(PROJECT_NAME MyThreads)
set(APP_NAME TestApp)

include_directories(include)
include_directories(src)
add_subdirectory(src)
