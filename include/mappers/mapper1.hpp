#include "cartridge.hpp"
#include "mapper.hpp"

class Mapper1 final : public Mapper {
public:
    Mapper1(Cartridge& cartridge, std::uint8_t prg_banks, std::uint8_t chr_banks);
    std::optional<std::uint32_t> cpu_memRead(std::uint16_t addr) const override;
    void cpu_memWrite(std::uint16_t addr, std::uint8_t val) override;
    std::uint32_t ppu_memRead(std::uint16_t addr) const override;
private:
    Cartridge& cartridge_;
    std::uint8_t shift_{0x10};
    std::uint8_t control_{0x0C};
    std::uint8_t chr_bank0_{0};
    std::uint8_t chr_bank1_{0};
    std::uint8_t prg_bank_{0};
};