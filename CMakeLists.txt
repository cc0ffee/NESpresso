cmake_minimum_required(VERSION 3.16)
project(nes_emu VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(nes
    src/main.cpp
)
target_include_directories(nes PRIVATE include)
