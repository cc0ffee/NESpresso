cmake_minimum_required(VERSION 3.16)
project(nes_emu VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(nes
    src/main.cpp
    src/cartridge.cpp
    src/bus.cpp
    src/cpu.cpp
    src/ppu.cpp
    src/mappers/mapper0.cpp
)
target_include_directories(nes PRIVATE include)
find_package(SDL2 REQUIRED CONFIG)
target_link_libraries(nes PRIVATE $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> SDL2::SDL2)
