#include "bus.hpp"
std::uint8_t Bus::cpu_memRead(std::uint16_t addr) { return cpu_ram_[addr]; }
void Bus::cpu_memWrite(std::uint16_t addr, std::uint8_t val) { cpu_ram_[addr] = val; }
