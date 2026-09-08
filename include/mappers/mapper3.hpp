#include "mapper.hpp"

class Mapper3 final : public Mapper {
public:
    Mapper3(std::uint8_t prg_banks, std::uint8_t chr_banks);
    std::optional<std::uint32_t> cpu_memRead(std::uint16_t addr) const override;
    void cpu_memWrite(std::uint16_t addr, std::uint8_t val) override;
    std::uint32_t ppu_memRead(std::uint16_t addr) const override;
private:
    std::uint8_t chr_bank_{0};
};