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
<<<<<<< HEAD

if(MSVC)
    target_compile_options(os_experiment PRIVATE /utf-8)
endif()
=======
>>>>>>> d43ace2f57b7d226bfea79c0fbde8a92a3578f75
