#include "mappers/mapper2.hpp"

Mapper2::Mapper2(std::uint8_t prg_banks, std::uint8_t chr_banks) : Mapper(prg_banks, chr_banks) {}

std::optional<std::uint32_t> Mapper2::cpu_memRead(std::uint16_t addr) const {

    if (addr < 0x8000) {
        return std::nullopt;
    }

    if (addr < 0xC000) {
        return ((prg_bank_ % prg_banks_) + (addr & 0x3FFF));
    }

    return ((prg_banks_ - 1) * 0x4000u) + (addr & 0x3FFF);
}

void Mapper2::cpu_memWrite(std::uint16_t addr, std::uint8_t val) {
    if (addr >= 0x8000) {
        prg_bank_ = val;
    }
}
