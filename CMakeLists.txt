cmake_minimum_required(VERSION 3.10)

project(arinc653_scheduler_sim VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(arinc653_scheduler_sim
    src/DisplaySystem.cpp
    src/FlightControl.cpp
    src/HealthMonitor.cpp
    src/Logger.cpp
    src/Scheduler.cpp
    src/main.cpp
)

target_include_directories(arinc653_scheduler_sim PRIVATE include)

if(MSVC)
    target_compile_options(arinc653_scheduler_sim PRIVATE /W4 /permissive-)
else()
    target_compile_options(arinc653_scheduler_sim PRIVATE -Wall -Wextra -Wpedantic)
endif()
