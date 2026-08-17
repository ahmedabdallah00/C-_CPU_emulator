cmake_minimum_required(VERSION 3.28)
project(C++_CPU_Simulator CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# CPU Simulator executable
add_executable(simulator
    src/main.cpp
    src/memory.cpp
    src/cpu.cpp
    src/instruction.cpp
)

target_include_directories(simulator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# CPU tests
add_executable(test_cpu
    tests/test_cpu.cpp
    src/memory.cpp
    src/cpu.cpp
    src/instruction.cpp
)

target_include_directories(test_cpu
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Memory tests
add_executable(test_memory
    tests/test_memory.cpp
    src/memory.cpp
)

target_include_directories(test_memory
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
