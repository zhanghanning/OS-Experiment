cmake_minimum_required(VERSION 3.16)

project(OSExperiment LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(os_experiment
    src/main.cpp
    src/MiniOS.cpp
    src/MemoryManager.cpp
    src/Process.cpp
    src/Scheduler.cpp
)

target_include_directories(os_experiment PRIVATE include)
