#include "bus.hpp"
#include <iostream>

Bus::Bus(Cartridge& cartridge, PPU& ppu) : cartridge_(cartridge), ppu_(ppu) {}

std::uint8_t Bus::cpu_memRead(std::uint16_t addr) {
    if (addr <= 0x1FFF) {
        return cpu_ram_[addr & 0x07FF];
    }

    if (addr >= 0x2000 && addr <= 0x3FFF) {
        const std::uint16_t reg_addr = 0x2000 | (addr & 0x0007);
        return ppu_.ppu_read(reg_addr);
    }

    if (addr >= 0x4000 && addr <= 0x401F) {
        return 0;
    }

    const auto cartridge_data =
        cartridge_.cpu_memRead(addr);

    if (cartridge_data) {
        return *cartridge_data;
    }

    return 0;
}

void Bus::cpu_memWrite(std::uint16_t addr, std::uint8_t val) {
        if (addr <= 0x1FFF) {
        cpu_ram_[addr & 0x07FF] = val;
        return;
    } else if (addr <= 0x3FFF) {
        const std::uint16_t reg_addr = 0x2000 | (addr & 0x0007);
        ppu_.ppu_writeReg(reg_addr, val);
        return;
    } else if (addr == 0x4014) {
        for (int i = 0; i < 256; i++) {
            ppu_.oam_data_[i] = cpu_memRead((static_cast<std::uint16_t>(val << 8) + i));
        }
    }
}

bool Bus::calc_nmi_vblank() {
    return ((ppu_.ctrl_ & 0x80) != 0) && ((ppu_.status_ & 0x80) != 0);
}
