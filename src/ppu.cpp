#include "ppu.hpp"
#include <iostream>

PPU::PPU(Cartridge& cartridge) : cartridge_(cartridge) {}

void PPU::ppu_writeReg(std::uint16_t addr, std::uint8_t val) {
    switch(addr) {
        case 0x2000:
            ctrl_ = val;
            reg_t_ = (reg_t_ & 0xF3FF) | (static_cast<std::uint16_t>(val & 0x03) << 10);
            break;
        case 0x2001:
            mask_ = val;
            break;
        case 0x2002:
            break;
        case 0x2003:
            oam_addr_ = val;
            break;
        case 0x2004:
            oam_data_[oam_addr_ & 0xFF] = val;
            oam_addr_ = (oam_addr_ + 1) & 0xFF;
            break;
        case 0x2005:
            if (!reg_w_) {
                ppu_scroll_fine_x_ = (val & 7);
                reg_t_ = static_cast<std::uint16_t>((reg_t_ & 0x7FE0) | (val >> 3));
            } else {
                reg_t_ = static_cast<std::uint16_t>((reg_t_ & 0x0C1F) | (((val & 0xF8) << 2) | ((val & 0x7) << 12)));
            }
            reg_w_ = !reg_w_;
            break;
        case 0x2006: {
            if (!reg_w_) {
                reg_t_ = (reg_t_ & 0x00FF) |(static_cast<uint16_t>((val & 0x3F)) << 8);
            } else {
                reg_t_ = (reg_t_ & 0x7F00) | static_cast<uint16_t>(val);
                reg_v_ = reg_t_;
            }
            reg_w_ = !reg_w_;
            break;
        };
        case 0x2007: {
            const std::uint16_t vram_addr = reg_v_ & 0x3FFF;
            if (vram_addr < 0x2000) {
                if (cartridge_.uses_chr_ram_) {
                    cartridge_.chr_memory_[vram_addr] = val;
                }
            } else if (vram_addr < 0x3F00) {

                if (cartridge_.mirroring_ == NameTableMirroring::Horizontal) {
                    nametable_ram_[(vram_addr & 0x03FF) | (vram_addr & 0x0800) >> 1] = val;
                } else {
                    nametable_ram_[(vram_addr & 0x07FF)] = val;
                }

            } else {
                if ((vram_addr & 0x3) == 0) {
                    palette_ram_[vram_addr & 0x0F] = val;
                } else {
                    palette_ram_[vram_addr & 0x1F] = val;
                }
            }
            reg_v_ += (ctrl_ & 0x04) ? 32 : 1;
            reg_v_ &= 0x3FFF;
            break;
        }
    }
}

std::uint8_t PPU::ppu_read(uint16_t addr) {
    switch(addr) {
        case 0x2000:
            break;
        case 0x2001:
            break;
        case 0x2002: {
            std::uint8_t result = status_ & 0xE0;
            status_ &= static_cast<std::uint8_t>(~0x80);
            reg_w_ = false;
            return result;
        }
        case 0x2003:
            break;
        case 0x2004:
            break;
        case 0x2005:
            break;
        case 0x2006:
            break;
        case 0x2007: {
            const std::uint16_t vram_addr = reg_v_ & 0x3FFF;
            uint8_t temp = ppu_read_buffer_;
            if (vram_addr < 0x2000) {
                ppu_read_buffer_ = cartridge_.chr_memory_[vram_addr];
            } else if (vram_addr < 0x3F00) {

                if (cartridge_.mirroring_ == NameTableMirroring::Horizontal) {
                    ppu_read_buffer_ = nametable_ram_[(vram_addr & 0x03FF) | (vram_addr & 0x0800) >> 1];
                } else {
                    ppu_read_buffer_ = nametable_ram_[(vram_addr & 0x07FF)];
                }

            } else {
                if ((vram_addr & 0x3) == 0) {
                    temp = palette_ram_[vram_addr & 0x0F];
                } else {
                    temp = palette_ram_[vram_addr & 0x1F];
                }
            }
            reg_v_ += (ctrl_ & 0x04) ? 32 : 1;
            reg_v_ &= 0x3FFF;
            return temp;
        }
    }
    return 0;
}

std::uint8_t PPU::ppu_directRead(std::uint16_t addr) {
    const std::uint16_t vram_addr = addr & 0x3FFF;

    if (vram_addr < 0x2000) {
        return cartridge_.chr_memory_[vram_addr];
    }

    if (vram_addr < 0x3F00) {
        if (cartridge_.mirroring_ == NameTableMirroring::Horizontal) {

            const std::uint16_t index =
                (vram_addr & 0x03FF) | ((vram_addr & 0x0800) >> 1);

            return nametable_ram_[index];
        }

        return nametable_ram_[vram_addr & 0x07FF];
    }

    if ((vram_addr & 0x03) == 0) {
        return palette_ram_[vram_addr & 0x0F];
    }

    return palette_ram_[vram_addr & 0x1F];
}


void PPU::step() {
    ++ppuDot;

    if (ppuDot >= 341) {
        ppuDot = 0;
        ++ppuScanline;

        if (ppuScanline >= 262) {
            ppuScanline = 0;
        }
    }
    if (ppuDot == 1 && ppuScanline == 241) {
        status_ |= 0x80;
        frame_ready_ = true;
    }


    else if (ppuDot == 1 && ppuScanline == 261) {
        status_ &= 0x1F;
        sprite_count_ = 0;
        scanline_contains_sprite_zero_ = false;
    }

}
