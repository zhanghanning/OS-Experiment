@echo off
cmake -S . -B build
cmake --build build
if exist build\Debug\os_experiment.exe (
    build\Debug\os_experiment.exe --demo
) else if exist build\Release\os_experiment.exe (
    build\Release\os_experiment.exe --demo
) else (
    build\os_experiment.exe --demo
)
