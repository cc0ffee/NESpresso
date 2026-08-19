#pragma once
#include "cartridge.hpp"
#include <array>

class Bus {
public:
    explicit Bus(Cartridge& cartridge);
    std::uint8_t cpu_memRead(std::uint16_t addr);
    void cpu_memWrite(std::uint16_t addr, std::uint8_t val);
    bool calc_nmi_vblank() { return false; }
private:
    Cartridge& cartridge_;
    std::array<std::uint8_t, 2048> cpu_ram_{};
};
