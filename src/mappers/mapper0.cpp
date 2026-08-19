#include "mappers/mapper0.hpp"

Mapper0::Mapper0(std::uint8_t prg_banks, std::uint8_t chr_banks) : Mapper(prg_banks, chr_banks) {}

std::optional<std::uint32_t> Mapper0::cpu_memRead(std::uint16_t addr) const {
    
    if (addr < 0x8000) {
        return std::nullopt;
    }

    if (prg_banks_ == 1) {
        return addr & 0x3FFF; // 16 KB mem
    }

    return addr & 0x7FFF; // 32 KB mem
}
