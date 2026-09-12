cmake_minimum_required(VERSION 3.16)

project(nespresso VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(FetchContent)

set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.9
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL2)

include(FetchContent)

FetchContent_Declare(
    nfd
    GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
    GIT_TAG master
)

FetchContent_MakeAvailable(nfd)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/libs/imgui)

add_executable(nes
    src/main.cpp
    src/cartridge.cpp
    src/bus.cpp
    src/cpu.cpp
    src/ppu.cpp
    src/mappers/mapper0.cpp
    src/mappers/mapper1.cpp
    src/mappers/mapper2.cpp
    src/mappers/mapper3.cpp
)

target_link_libraries(nes PRIVATE
    SDL2::SDL2
    nfd
)

target_include_directories(nes PRIVATE
    include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${SDL2_SOURCE_DIR}/include
)

target_compile_features(nes PRIVATE cxx_std_20)

set_target_properties(nes PROPERTIES
    CXX_EXTENSIONS OFF
)

add_library(imgui
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp

    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${SDL2_SOURCE_DIR}/include
)

target_link_libraries(imgui PUBLIC
    SDL2::SDL2
)

target_link_libraries(nes PRIVATE
    SDL2::SDL2
    imgui
)

if(MSVC)
    target_compile_options(nes PRIVATE
        /W4
        /permissive-
    )
else()
    target_compile_options(nes PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

add_custom_command(TARGET nes POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2::SDL2>
        $<TARGET_FILE_DIR:nes>
)