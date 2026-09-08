cmake_minimum_required(VERSION 3.16)

project(nes_emu VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(SDL2 REQUIRED CONFIG)

add_executable(nes
    src/main.cpp
    src/cartridge.cpp
    src/bus.cpp
    src/cpu.cpp
    src/ppu.cpp
    src/mappers/mapper0.cpp
    src/mappers/mapper2.cpp
    src/mappers/mapper3.cpp
)

target_include_directories(nes PRIVATE include)
target_compile_features(nes PRIVATE cxx_std_20)
set_target_properties(nes PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(nes PRIVATE
    SDL2::SDL2
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
)

if(MSVC)
    target_compile_options(nes PRIVATE /W4 /permissive-)
else()
    target_compile_options(nes PRIVATE -Wall -Wextra -Wpedantic)
endif()