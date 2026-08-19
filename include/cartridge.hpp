#pragma once
#include "mapper.hpp"
#include <fstream>
#include <vector>
#include <cstdint>
#include <optional>
#include <memory>

enum class NameTableMirroring {
    Horizontal,
    Vertical,
    FourScreen
};

class Cartridge {
public:
    explicit Cartridge(std::ifstream& rom);
    std::optional<uint8_t> cpu_memRead(std::uint16_t addr);
    void cpu_memWrite(std::uint16_t addr, std::uint8_t val);
    NameTableMirroring mirroring_ = NameTableMirroring::Horizontal;
    const std::vector<std::uint8_t>& chr_memory() const {
        return chr_memory_;
    }

    std::vector<std::uint8_t> chr_memory_;

    bool has_battery_ = false;
    bool has_trainer_ = false;
    bool uses_chr_ram_ = false;

private:
    std::vector<std::uint8_t> prg_rom_;

    std::uint8_t mapper_id_ = 0;
    std::unique_ptr<Mapper> mapper_;
};