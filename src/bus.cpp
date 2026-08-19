#include "bus.hpp"
Bus::Bus(Cartridge& cartridge) : cartridge_(cartridge) {}
std::uint8_t Bus::cpu_memRead(std::uint16_t addr) {
    if (addr <= 0x1FFF) { return cpu_ram_[addr & 0x07FF]; }
    return cartridge_.cpu_memRead(addr).value_or(0);
}
void Bus::cpu_memWrite(std::uint16_t addr, std::uint8_t val) {
    if (addr <= 0x1FFF) { cpu_ram_[addr & 0x07FF] = val; }
}
