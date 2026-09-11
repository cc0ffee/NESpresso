#include "mappers/mapper1.hpp"
#include "cartridge.hpp"

Mapper1::Mapper1(Cartridge& cartridge, std::uint8_t prg_banks, std::uint8_t chr_banks) : Mapper(prg_banks, chr_banks), cartridge_(cartridge) {}

std::optional<std::uint32_t> Mapper1::cpu_memRead(std::uint16_t addr) const {
    
    if (addr < 0x8000) {
        return std::nullopt;
    }

    switch ((control_ >> 2) & 0x03) {
        case 0:
        case 1:
            return ((prg_bank_ & 0x0E) * 0x4000u) + (addr & 0x7FFF);
        case 2: {
            if (addr < 0xC000) {
                return addr & 0x3FFF;
            }

            return (((prg_bank_ & 0x0F) % prg_banks_) * 0x4000u) + (addr & 0x3FFF);
        }
        default:
            if (addr < 0xC000) {
                return (((prg_bank_ & 0x0F) % prg_banks_) * 0x4000u) + (addr & 0x3FFF);
            }

            return ((prg_banks_ - 1) * 0x4000u) + (addr & 0x3FFF);
    }
}

void Mapper1::cpu_memWrite(std::uint16_t addr, std::uint8_t val) {
    if (addr < 0x8000) {
        return;
    }

    if (val & 0x80) {
        shift_ = 0x10;
        control_ |= 0x0C;
        return;
    }

        bool const complete = shift_ & 1;
    shift_ >>= 1;
    shift_ |= (val & 0x01) << 4;

    if (!complete) {
        return;
    }

    std::uint8_t const data = shift_ & 0x1F;

    switch ((addr >> 13) & 0x03) {
        case 0:
            control_ = data;
            switch (control_ & 0x03) {
                case 0:
                    cartridge_.mirroring_ = NameTableMirroring::SingleScreenLower;
                    break;
                case 1:
                    cartridge_.mirroring_ = NameTableMirroring::SingleScreenUpper;
                    break;
                case 2:
                    cartridge_.mirroring_ = NameTableMirroring::Vertical;
                    break;
                case 3:
                    cartridge_.mirroring_ = NameTableMirroring::Horizontal;
                    break;
            }
            break;
        case 1:
            chr_bank0_ = data;
            break;
        case 2:
            chr_bank1_ = data;
            break;
        case 3:
            prg_bank_ = data;
            break;
    }
    shift_ = 0x10;
}

std::uint32_t Mapper1::ppu_memRead(std::uint16_t addr) const {
    const std::uint32_t chr_size = chr_banks_ ? chr_banks_ * 0x2000u : 0x2000u;
    if (!(control_ & 0x10)) {
        return (((chr_bank0_ & 0x1E) * 0x1000u) + (addr & 0x1FFF)) % chr_size;
    }

    if (addr < 0x1000) {
        return ((chr_bank0_ * 0x1000u) + (addr & 0x0FFF)) % chr_size;
    }

    return ((chr_bank1_ * 0x1000u) + (addr & 0x0FFF)) % chr_size;
}