#define SDL_MAIN_HANDLED

#include "bus.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"
#include "ppu.hpp"
#include "opcodes.hpp"

#include <SDL2/SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include <nfd.hpp>

#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <memory>

static bool load_rom(const std::string& path, std::unique_ptr<std::ifstream>& rom_file, std::unique_ptr<Cartridge>& cartridge, std::unique_ptr<PPU>& ppu, std::unique_ptr<Bus>& bus, std::unique_ptr<CPU>& cpu) {
    auto new_rom_file = std::make_unique<std::ifstream>(path, std::ios::binary);

    if (!*new_rom_file) {
        return false;
    }

    auto new_cartridge = std::make_unique<Cartridge>(*new_rom_file);
    auto new_ppu = std::make_unique<PPU>(*new_cartridge);
    auto new_bus = std::make_unique<Bus>(*new_cartridge, *new_ppu);
    auto new_cpu = std::make_unique<CPU>(*new_bus);

    new_cpu->reset();

    rom_file = std::move(new_rom_file);
    cartridge = std::move(new_cartridge);
    ppu = std::move(new_ppu);
    bus = std::move(new_bus);
    cpu = std::move(new_cpu);

    return true;
}

struct Color {
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;

    std::uint32_t argb() const {
        return 0xFF000000u |
            (static_cast<std::uint32_t>(red) << 16) |
            (static_cast<std::uint32_t>(green) << 8) |
            static_cast<std::uint32_t>(blue);
    }
};


std::uint8_t Pal[] = {
0x65, 0x65, 0x65, 0x00, 0x2A, 0x84, 0x15, 0x13, 0xA2, 0x3A, 0x01, 0x9E, 0x59, 0x00, 0x7A, 0x6A, 0x00, 0x3E, 0x68, 0x08, 0x00, 0x53, 0x1D, 0x00, 0x32, 0x34, 0x00, 0x0D, 0x46, 0x00, 0x00, 0x4F, 0x00, 0x00, 0x4C, 0x09, 0x00, 0x3F, 0x4B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xAE, 0xAE, 0xAE, 0x17, 0x5F, 0xD6, 0x43, 0x41, 0xFF, 0x75, 0x29, 0xFA, 0x9E, 0x1D, 0xCA, 0xB4, 0x20, 0x7B, 0xB1, 0x33, 0x22, 0x96, 0x4E, 0x00, 0x6A, 0x6C, 0x00, 0x39, 0x84, 0x00, 0x0F, 0x90, 0x00, 0x00, 0x8D, 0x33, 0x00, 0x7B, 0x8C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xFE, 0xFE, 0xFE, 0x66, 0xAF, 0xFF, 0x93, 0x90, 0xFF, 0xC5, 0x78, 0xFF, 0xEE, 0x6C, 0xFF, 0xFF, 0x6F, 0xCA, 0xFF, 0x82, 0x71, 0xE6, 0x9E, 0x25, 0xBA, 0xBC, 0x00, 0x88, 0xD5, 0x01, 0x5E, 0xE1, 0x32, 0x47, 0xDD, 0x82, 0x4A, 0xCB, 0xDC, 0x4E, 0x4E, 0x4E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xFE, 0xFE, 0xFE, 0xC0, 0xDE, 0xFF, 0xD2, 0xD1, 0xFF, 0xE7, 0xC7, 0xFF, 0xF8, 0xC2, 0xFF, 0xFF, 0xC3, 0xE9, 0xFF, 0xCB, 0xC4, 0xF5, 0xD7, 0xA5, 0xE2, 0xE3, 0x94, 0xCE, 0xED, 0x96, 0xBC, 0xF2, 0xAA, 0xB3, 0xF1, 0xCB, 0xB4, 0xE9, 0xF0, 0xB6, 0xB6, 0xB6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

Color Palette[64];

constexpr int bitmap_width = 256;
constexpr int bitmap_height = 240;

std::vector<std::uint32_t> make_screen_bitmap(const PPU& ppu) {
    const auto& framebuffer = ppu.framebuffer_;

    std::vector<std::uint32_t> pixels(bitmap_width * bitmap_height);

    for (std::size_t i = 0; i < pixels.size(); ++i) {
        const std::uint8_t color_index =
            framebuffer[i] & 0x3F;

        pixels[i] = Palette[color_index].argb();
    }

    return pixels;
}

int main() {

    for (int j = 0; j < 64; ++j) {
        Palette[j] = Color{Pal[j * 3], Pal[j * 3 + 1], Pal[j * 3 + 2]};
    }

    std::unique_ptr<std::ifstream> rom_file;
    std::unique_ptr<Cartridge> cartridge;
    std::unique_ptr<PPU> ppu;
    std::unique_ptr<Bus> bus;
    std::unique_ptr<CPU> cpu;


    SDL_Init(SDL_INIT_VIDEO);
    NFD::Guard nfdGuard;
    SDL_Window* window_ = SDL_CreateWindow("NES Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, bitmap_width * 3, bitmap_height * 3, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, bitmap_width, bitmap_height);
    SDL_SetTextureScaleMode(texture_, SDL_ScaleModeNearest);

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = false;

    ImGui_ImplSDL2_InitForSDLRenderer(window_, renderer_);
    ImGui_ImplSDLRenderer2_Init(renderer_);


    std::vector<std::uint32_t> pixels(bitmap_width * bitmap_height,0xFF000000);
    bool running = true;

    std::uint64_t frame_count = 0;
    const auto frame_duration =
    std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(1.0 / 60.0));
    auto next_frame = std::chrono::steady_clock::now();

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT)
                running = false;
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open ROM")) {
                NFD::UniquePath outPath;
                nfdfilteritem_t filters[] = { "NES ROM", "nes" };
                nfdresult_t result = NFD::OpenDialog(outPath, filters, 1);
                if (result == NFD_OKAY) {
                    if (!load_rom(std::string(outPath.get()), rom_file, cartridge, ppu, bus, cpu)) {
                        std::cerr << "Failed to load ROM\n";
                    }
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                running = false;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();

        if (bus && ppu && cpu) {
            const Uint8* keys = SDL_GetKeyboardState(nullptr);

            bus->controller1_buttons_ = 0;

            if (keys[SDL_SCANCODE_Z]) {
                bus->controller1_buttons_ |= 0x01;
            }
            if (keys[SDL_SCANCODE_X]) {
                bus->controller1_buttons_ |= 0x02;
            }
            if (keys[SDL_SCANCODE_RSHIFT]) {
                bus->controller1_buttons_ |= 0x04;
            }
            if (keys[SDL_SCANCODE_RETURN]) {
                bus->controller1_buttons_ |= 0x08;
            }
            if (keys[SDL_SCANCODE_UP]) {
                bus->controller1_buttons_ |= 0x10;
            }
            if (keys[SDL_SCANCODE_DOWN]) {
                bus->controller1_buttons_ |= 0x20;
            }
            if (keys[SDL_SCANCODE_LEFT]) {
                bus->controller1_buttons_ |= 0x40;
            }
            if (keys[SDL_SCANCODE_RIGHT]) {
                bus->controller1_buttons_ |= 0x80;
            }

            if (!running) {
                break;
            }

            while (!ppu->frame_ready_ && !cpu->cpu_halt_) {
                const int cpu_cycles = cpu->step();

                for (int i = 0; i < cpu_cycles * 3; ++i) {
                    ppu->step();
                }
            }

            if (!ppu->frame_ready_) {
                continue;
            }

            ppu->frame_ready_ = false;
            ++frame_count;

            pixels = make_screen_bitmap(*ppu);
        }
        SDL_UpdateTexture(texture_, nullptr, pixels.data(), bitmap_width * sizeof(std::uint32_t));

        SDL_RenderClear(renderer_);
        SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer_);

        SDL_RenderPresent(renderer_);
        next_frame += frame_duration;
        std::this_thread::sleep_until(next_frame);
        
        if (std::chrono::steady_clock::now() > next_frame + frame_duration) {
            next_frame = std::chrono::steady_clock::now();
        }
    }

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyTexture(texture_);
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
    return 0;

}