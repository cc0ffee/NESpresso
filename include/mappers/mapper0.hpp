#include "mapper.hpp"

class Mapper0 final : public Mapper {
public:
    Mapper0(std::uint8_t prg_banks, std::uint8_t chr_banks);
    std::optional<std::uint32_t> cpu_memRead(std::uint16_t addr) const override;
};