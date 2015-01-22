# required cmake version
cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_BUILD_TYPE Debug)

# project name
set(PROJECT_NAME MyThreads)
set(APP_NAME TestApp)

include_directories(include)
include_directories(src)
add_subdirectory(src)
