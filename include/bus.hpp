#pragma once
#include "cartridge.hpp"
#include "ppu.hpp"
#include <array>

class Bus {
public:
    explicit Bus(Cartridge& cartridge, PPU& ppu);

    std::uint8_t cpu_memRead(std::uint16_t addr);
    void cpu_memWrite(std::uint16_t addr, std::uint8_t val); 

    bool calc_nmi_vblank();
    std::uint8_t controller1_buttons_{0};
    std::uint8_t controller2_buttons_{0};

private:
    Cartridge& cartridge_;
    PPU& ppu_;
    std::array<std::uint8_t, 2048> cpu_ram_{};
    std::uint8_t controller1_shift_{0};
    std::uint8_t controller2_shift_{0};
    bool controller_strobe_{false};
};