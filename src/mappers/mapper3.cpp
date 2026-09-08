#include "mappers/mapper3.hpp"

Mapper3::Mapper3(std::uint8_t prg_banks, std::uint8_t chr_banks) : Mapper(prg_banks, chr_banks) {}

std::optional<std::uint32_t> Mapper3::cpu_memRead(std::uint16_t addr) const {
    
    if (addr < 0x8000) {
        return std::nullopt;
    }

    if (prg_banks_ == 1) {
        return addr & 0x3FFF;
    }

    return addr & 0x7FFF;
}

void Mapper3::cpu_memWrite(std::uint16_t addr, std::uint8_t val) {
    if (addr >= 0x8000) {
        chr_bank_ = val & 0x3;
    }
}

std::uint32_t Mapper3::ppu_memRead(std::uint16_t addr) const {
    if (chr_banks_ == 0) {
        return addr & 0x1FFF;
    }

    return ((chr_bank_ % chr_banks_) * 0x2000u) + (addr & 0x1FFF);
}