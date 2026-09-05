#include "cartridge.hpp"
#include <iostream>
#include <string>
#include "mappers/mapper0.hpp"
#include "mappers/mapper2.hpp"

namespace INesFlags6 {
    constexpr std::uint8_t vertical_mirroring = 1U << 0;
    constexpr std::uint8_t battery = 1U << 1;
    constexpr std::uint8_t trainer = 1U << 2;
    constexpr std::uint8_t four_screen = 1U << 3;
    constexpr std::uint8_t mapper_lower = 0xF0;
}

constexpr bool has_flag(std::uint8_t value, std::uint8_t mask) {
    return (value & mask) != 0;
}

Cartridge::Cartridge(std::ifstream& rom) {
    
    std::vector<std::uint8_t> rom_data(
        (std::istreambuf_iterator<char>(rom)),
        std::istreambuf_iterator<char>()
    );

    if (rom_data[0] != 'N' || rom_data[1] != 'E' || rom_data[2] != 'S' || rom_data[3] != 0x1A) {
        throw std::runtime_error("Invalid iNES header");
    }

    const std::uint8_t prg_banks = rom_data[4];
    const std::uint8_t chr_banks = rom_data[5];

    const std::uint8_t flags_6 = rom_data[6];
    const std::uint8_t flags_7 = rom_data[7];
    
    has_battery_ = has_flag(flags_6, INesFlags6::battery);
    has_trainer_ = has_flag(flags_6,  INesFlags6::trainer);

    const bool four_screen = has_flag(flags_6, INesFlags6::four_screen);
    const bool vertical = has_flag(flags_6,INesFlags6::vertical_mirroring);
    if (four_screen) { mirroring_ = NameTableMirroring::FourScreen; } 
    else if (vertical) { mirroring_ = NameTableMirroring::Vertical; } 
    else { mirroring_ = NameTableMirroring::Horizontal; }

    const std::uint8_t mapper_lower = (flags_6 & INesFlags6::mapper_lower) >> 4;
    const std::uint8_t mapper_upper = flags_7 & 0xF0;
    mapper_id_ = mapper_upper | mapper_lower;

    switch (mapper_id_) {
        case 0:
            mapper_ = std::make_unique<Mapper0>(prg_banks, chr_banks);
            break;
        case 2:
            mapper_ = std::make_unique<Mapper2>(prg_banks, chr_banks);
            break;
        default:
            throw std::runtime_error("Unsupported mapper: " + std::to_string(mapper_id_));
    }

    std::size_t offset = 16;

    if (has_trainer_) {
        offset += 512;
    }

    const std::size_t prg_size = static_cast<std::size_t>(prg_banks) * 16 * 1024;
    prg_rom_.assign(rom_data.begin() + offset, rom_data.begin() + offset + prg_size);

    offset += prg_size;

    const std::size_t chr_size = static_cast<std::size_t>(chr_banks) * 8 * 1024;

    if (chr_size > 0) {
        chr_memory_.assign(rom_data.begin() + offset, rom_data.begin() + offset + chr_size);
    } else {
        chr_memory_.resize(8 * 1024);
        uses_chr_ram_ = true;
    }

}

std::optional<uint8_t> Cartridge::cpu_memRead(std::uint16_t addr) {
    const auto mapped_addr = mapper_->cpu_memRead(addr);

    if (!mapped_addr) {
        return std::nullopt;
    }

    return prg_rom_.at(*mapped_addr);
}

void Cartridge::cpu_memWrite(std::uint16_t addr, std::uint8_t val) {
    mapper_->cpu_memWrite(addr, val);
}