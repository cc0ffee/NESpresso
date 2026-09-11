#include "bus.hpp"
#include <iostream>

Bus::Bus(Cartridge& cartridge, PPU& ppu) : cartridge_(cartridge), ppu_(ppu) {}

std::uint8_t Bus::cpu_memRead(std::uint16_t addr) {
    if (addr <= 0x1FFF) {
        open_bus_= cpu_ram_[addr & 0x07FF];
        return open_bus_;
    }

    if (addr >= 0x2000 && addr <= 0x3FFF) {
        const std::uint16_t reg_addr = 0x2000 | (addr & 0x0007);
        open_bus_ = ppu_.ppu_read(reg_addr);
        return open_bus_;
    }

    if (addr == 0x4016) {
        if (controller_strobe_) {
            const std::uint8_t val = controller1_buttons_ & 1;
            open_bus_ = (open_bus_ & 0xE0) | val;
            return open_bus_;
        }
        const std::uint8_t controller_bit = controller1_shift_ & 1;
        controller1_shift_ = static_cast<std::uint8_t>( (controller1_shift_ >> 1) | 0x80);
        open_bus_ = (open_bus_ & 0xE0) | controller_bit;
        return open_bus_; 
    }

    if (addr == 0x4017) {
        if (controller_strobe_) {
            const std::uint8_t val = controller2_buttons_ & 1;
            open_bus_ = (open_bus_ & 0xE0) | val;
            return open_bus_;
        }

        const std::uint8_t controller_bit = controller2_shift_ & 1;
        controller2_shift_ = (controller2_shift_ >> 1) | 0x80;
        open_bus_ = (open_bus_ & 0xE0) | controller_bit;
        return open_bus_;
    }

    if (addr >= 0x4000 && addr <= 0x401F) {
        return open_bus_;
    }

    const auto cartridge_data = cartridge_.cpu_memRead(addr);

    if (cartridge_data) {
        open_bus_ = *cartridge_data;
        return open_bus_;
    }

    return open_bus_;
}

void Bus::cpu_memWrite(std::uint16_t addr, std::uint8_t val) {
    open_bus_ = val;
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
        return;
    } else if (addr == 0x4016) {
        if (controller_strobe_ || (val & 1)) {
            controller1_shift_ = controller1_buttons_;
            controller2_shift_ = controller2_buttons_;
        }

        controller_strobe_ = (val & 1) != 0;
        return;
    } else if (addr >= 0x8000) {
        cartridge_.cpu_memWrite(addr, val);
        return;
    }
}

bool Bus::calc_nmi_vblank() {
    return ((ppu_.ctrl_ & 0x80) != 0) && ((ppu_.status_ & 0x80) != 0);
}