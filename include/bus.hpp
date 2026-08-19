#pragma once
#include <array>
#include <cstdint>

class Bus {
public:
    std::uint8_t cpu_memRead(std::uint16_t addr);
    void cpu_memWrite(std::uint16_t addr, std::uint8_t val);
    bool calc_nmi_vblank() { return false; }
private:
    std::array<std::uint8_t, 65536> cpu_ram_{};
};
