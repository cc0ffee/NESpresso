#pragma once
#include <cstdint>
#include <optional>

class Mapper {
public:
    Mapper(std::uint8_t prg_banks, std::uint8_t chr_banks) : prg_banks_(prg_banks), chr_banks_(chr_banks) {}

    virtual ~Mapper() = default;
    virtual std::optional<std::uint32_t> cpu_memRead(std::uint16_t addr) const = 0;
    virtual void cpu_memWrite(std::uint16_t addr, std::uint8_t val) {}

protected:
    std::uint8_t prg_banks_;
    std::uint8_t chr_banks_;
};
