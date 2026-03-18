cmake_minimum_required(VERSION 3.10)

# Project name and language
project(riscv_emulator LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Optional: Add compiler warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Collect all source files
set(SOURCE_FILES
    main.c
    src/cpu.c
    src/bus.c
    src/dram.c
)

# Create executable
add_executable(riscv_emulator ${SOURCE_FILES})

# Add include directories to the target
target_include_directories(riscv_emulator PRIVATE include)
