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
                ppu_read_buffer_ = cartridge_.ppu_memRead(vram_addr);
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
        return cartridge_.ppu_memRead(vram_addr);;
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

    if (ppuScanline == 30 &&
        ppuDot == 1 &&
        (mask_ & 0x18) == 0x18) {

        status_ |= 0x40;
    }

    if ((ppuScanline < 240) || ppuScanline == 261) {
        if ((ppuDot > 0 && ppuDot <= 256) || (ppuDot > 320 && ppuDot <= 336)) {
            if (mask_ & 0x18) {
                if (mask_ & 0x08) {
                    bg_pattern_low_shift_ <<= 1;
                    bg_pattern_high_shift_ <<= 1;
                    bg_attribute_low_shift_ <<= 1;
                    bg_attribute_high_shift_ <<= 1;
                }

                if (ppuScanline < 240 && ppuDot > 1 && ppuDot <= 256) {
                    for (int i = 0; i < sprite_count_; ++i) {
                        if (sprite_x_pos_[i] > 0) {
                            --sprite_x_pos_[i];
                        } else {
                            sprite_pattern_low_shift_[i] <<= 1;
                            sprite_pattern_high_shift_[i] <<= 1;
                        }
                    }
                }

                const std::uint8_t cycleTick = static_cast<std::uint8_t>((ppuDot - 1) & 0x07);

                switch(cycleTick) {
                    case 0:
                        bg_pattern_low_shift_ = static_cast<std::uint16_t>((bg_pattern_low_shift_ & 0xFF00) | bg_pattern_low_bitplane_);
                        bg_pattern_high_shift_ = static_cast<std::uint16_t>((bg_pattern_high_shift_ & 0xFF00) | bg_pattern_high_bitplane_);
                        bg_attribute_low_shift_ = static_cast<std::uint16_t>((bg_attribute_low_shift_ & 0xFF00) | ((tile_attr_ & 0x01) ? 0xFF : 0x00));
                        bg_attribute_high_shift_= static_cast<std::uint16_t>((bg_attribute_high_shift_ & 0xFF00) | ((tile_attr_ & 0x02) ? 0xFF : 0x00));
                        ppu_address_bus_ = static_cast<std::uint16_t>(0x2000 | (reg_v_ & 0x0FFF));
                        ppu_temp_ = ppu_directRead(ppu_address_bus_);
                        break;
                    case 1:
                        ppu_nextChar = ppu_temp_;
                        break;    
                    case 2:
                        ppu_address_bus_ = static_cast<std::uint16_t>(0x23C0 | (reg_v_ & 0x0C00) | ((reg_v_ >> 4) & 0x38) | ((reg_v_ >> 2) & 0x07));
                        ppu_temp_ = ppu_directRead(ppu_address_bus_);
                        break;
                    case 3:
                        tile_attr_ = ppu_temp_;
                        if ((reg_v_ & 0x3) >= 0x2) {
                            tile_attr_ = static_cast<std::uint8_t>(tile_attr_ >> 2);
                        }
                        if ((((reg_v_ & 0b0000001111100000) >> 5) & 3) >= 2) {
                            tile_attr_ = static_cast<std::uint8_t>(tile_attr_ >> 4);
                        }
                        tile_attr_ = static_cast<std::uint8_t>(tile_attr_ & 3);
                        break;
                    case 4:
                        ppu_address_bus_ = static_cast<std::uint16_t>(((ctrl_ & 0x10) ? 0x1000 : 0x0000) | (ppu_nextChar << 4) | ((reg_v_ >> 12) & 0x07));
                        ppu_temp_ = ppu_directRead(ppu_address_bus_);
                        break;
                    case 5:
                        bg_pattern_low_bitplane_ = ppu_temp_;
                        ppu_address_bus_ += 8;
                        break;
                    case 6:
                        ppu_temp_ = ppu_directRead(ppu_address_bus_);
                        break;
                    case 7:
                        bg_pattern_high_bitplane_ = ppu_temp_;
                        if ((reg_v_ & 0x001F) == 31) {
                            reg_v_ &= static_cast<std::uint16_t>(~0x001F);
                            reg_v_ ^= 0x0400;
                        } else {
                            ++reg_v_;
                        }
                        break;
                }
            }
        }
    }

    if (ppuScanline < 240 && ppuDot > 0 && ppuDot <= 256) {
        std::uint8_t PalHi = 0;
        std::uint8_t PalLo = 0;

        if (((mask_ & 0x08) != 0) && (ppuDot > 8 || ((mask_ & 0x02) != 0))) {
            const int shift = 15 - ppu_scroll_fine_x_;
            std::uint8_t col0 = static_cast<std::uint8_t>(bg_pattern_low_shift_ >> shift & 1);
            std::uint8_t col1 = static_cast<std::uint8_t>(bg_pattern_high_shift_ >> shift & 1);
            PalLo = static_cast<std::uint8_t>((col1 << 1) | col0);

            std::uint8_t pal0 = static_cast<std::uint8_t>(bg_attribute_low_shift_ >> shift & 1);
            std::uint8_t pal1 = static_cast<std::uint8_t>(bg_attribute_high_shift_ >> shift & 1);
            PalHi = static_cast<std::uint8_t>((pal1 << 1) | pal0);

            if (PalLo == 0 && PalHi != 0) {
                PalHi = 0;
            }
        }

        std::uint8_t spritePalHi = 0;
        std::uint8_t spritePalLo = 0;
        bool spritePriority = false;

        if ((mask_ & 0x10) && (ppuDot > 8 || (mask_ & 0x04))) {
            for (int i = 0; i < sprite_count_; ++i) {
                if (sprite_x_pos_[i] != 0) {
                    continue;
                }
                bool spritePixelL = ((sprite_pattern_low_shift_[i] & 0x80) != 0);
                bool spritePixelH = ((sprite_pattern_high_shift_[i] & 0x80) != 0);
                spritePalLo = 0;
                if (spritePixelL) { spritePalLo = 1; }
                if (spritePixelH) { spritePalLo |= 2; }

                if (spritePalLo == 0) {
                    continue;
                }

                spritePalHi = static_cast<uint8_t>((sprite_attr_[i] & 0x03) | 0x04);
                spritePriority = ((sprite_attr_[i] >> 5) & 1) == 0;

                if (i == 0 && scanline_contains_sprite_zero_ && PalLo != 0 && ppuDot < 256) {
                    status_ |= 0x40;
                }
                break;
            }
        }

        if ((spritePriority && spritePalLo != 0) || PalLo == 0) {
            PalLo = spritePalLo;
            PalHi = spritePalHi;
            if (PalLo == 0) {
                PalHi = 0;
            }
        }

        const std::uint8_t palette_address = static_cast<std::uint8_t>((PalHi << 2) | PalLo);
        const std::size_t pixel_index = static_cast<std::size_t>(ppuScanline) * screen_width + static_cast<std::size_t>(ppuDot - 1);
        framebuffer_[pixel_index] = palette_ram_[palette_address] & 0x3F;
    } 

    if ((mask_ & 0x18) != 0 && ppuScanline < 240) {
        spriteEvaluation();
    }   

    if (((mask_ & 0x18) != 0) && (ppuScanline < 240 || ppuScanline == 261)) {
        if (ppuDot == 256) {
            incScrollY();
        } else if (ppuDot == 257) {
            resetXScroll();
        }

        if (ppuDot >= 280 && ppuDot <= 304 && ppuScanline == 261) {
            resetYScroll();
        }
    }
}

void PPU::incScrollY() {
    if ((reg_v_ & 0x7000) != 0x7000) {
        reg_v_ += 0x1000;
    } else {
        reg_v_ &= 0x0FFF;
        int y = (reg_v_ & 0x03E0) >> 5;
        if (y == 29) {
            y = 0;
            reg_v_ ^= 0x0800;
        } else {
            y++;
            y &= 0x1F;
        }
        reg_v_ = static_cast<uint16_t>((reg_v_ & 0xFC1F) | (y << 5));
    }
}

void PPU::resetXScroll() {
    reg_v_ = (reg_v_ & 0b0111101111100000) | (reg_t_ & 0b0000010000011111);
}

void PPU::resetYScroll() {
    reg_v_ = (reg_v_ & 0b0000010000011111) | (reg_t_ & 0b0111101111100000);
}

void PPU::spriteEvaluation() {
    if (ppuDot == 1) {
        secondary_oam_addr_ = 0;
        oam_addr_ = 0;
        sprite_eval_tick_ = 0;
        next_sprite_count_ = 0;
        next_scanline_contains_sprite_zero_ = false;
    }
    if (ppuDot > 0 && ppuDot <= 64) {
        if ((ppuDot & 1) == 1) {
            sprite_eval_temp_ = 0xFF;
        } else {
            secondary_oam_[secondary_oam_addr_] =
                sprite_eval_temp_;

            secondary_oam_addr_++;
            secondary_oam_addr_ &= 0x1F;
        }
    } else if (ppuDot > 64 && ppuDot <= 256) {
        if (oam_addr_ < oam_data_.size()) {
            if ((ppuDot & 1) == 1) {
                sprite_eval_temp_ = oam_data_[oam_addr_];
            } else {
                if (secondary_oam_addr_ < secondary_oam_.size()) {
                    secondary_oam_[secondary_oam_addr_] =
                        sprite_eval_temp_;
                }

                if (sprite_eval_tick_ == 0) {
                    if (ppuScanline - sprite_eval_temp_ >= 0 &&
                        ppuScanline - sprite_eval_temp_ <
                            ((ctrl_ & 0x20) ? 16 : 8)) {

                        if (secondary_oam_addr_ < secondary_oam_.size()) {
                            if (oam_addr_ == 0) {
                                next_scanline_contains_sprite_zero_ = true;
                            }

                            secondary_oam_addr_++;
                            oam_addr_++;
                            sprite_eval_tick_++;
                        } else {
                            status_ |= 0x20;
                            oam_addr_ += 4;
                        }
                    } else {
                        oam_addr_ += 4;
                    }
                } else {
                    secondary_oam_addr_++;
                    oam_addr_++;
                    sprite_eval_tick_++;

                    if (sprite_eval_tick_ == 4) {
                        sprite_eval_tick_ = 0;
                        next_sprite_count_++;
                    }
                }
            }
        }
    } else if (ppuDot >= 257 && ppuDot <= 320) {
        if (ppuDot == 257) {
            sprite_count_ = next_sprite_count_;
            scanline_contains_sprite_zero_ =
            next_scanline_contains_sprite_zero_;
        }
        switch ((ppuDot - 257) & 7) {
            case 0:
                sprite_y_pos_[(ppuDot - 257) >> 3] = secondary_oam_[((ppuDot - 257) >> 3) * 4];
                break;
            case 1:
                sprite_tile_[(ppuDot - 257) >> 3] = secondary_oam_[((ppuDot - 257) >> 3) * 4 + 1];
                break;
            case 2:
                sprite_attr_[(ppuDot - 257) >> 3] = secondary_oam_[((ppuDot - 257) >> 3) * 4 + 2];
                break;
            case 3:
                sprite_x_pos_[(ppuDot - 257) >> 3] = secondary_oam_[((ppuDot - 257) >> 3) * 4 + 3];
                break;
            case 4: {
                const int slot = ((ppuDot - 257) >> 3);

                if ((ctrl_ & 0x20) == 0) {
                    int row = (ppuScanline - sprite_y_pos_[slot]) & 7;
                    if (sprite_attr_[slot] & 0x80) {
                        row = 7 - row;
                    }
                    ppu_address_bus_ = static_cast<std::uint16_t>(((ctrl_ & 0x08) ? 0x1000 : 0x0000) + (sprite_tile_[slot] * 16) + row);
                } else {
                    int row = (ppuScanline - sprite_y_pos_[slot]) & 15;
                    if (sprite_attr_[slot] & 0x80) {
                        row = 15 - row;
                    }
                    ppu_address_bus_ = static_cast<std::uint16_t>( ((sprite_tile_[slot] & 1) ? 0x1000 : 0x0000) + ((sprite_tile_[slot] & 0xFE) * 16) + ((row >= 8) ? 16 : 0) + (row & 7));
                }

                break;
            }
            case 5:
                sprite_eval_temp_ = ppu_directRead(ppu_address_bus_);
                if (ppuScanline == 261) {
                    sprite_eval_temp_ = 0;
                }

                if (((sprite_attr_[((ppuDot - 257) >> 3)] >> 6) & 1) == 1) {
                    sprite_eval_temp_ = static_cast<std::uint8_t>(((sprite_eval_temp_ & 0xF0) >> 4) | ((sprite_eval_temp_ & 0xF) << 4));
                    sprite_eval_temp_ = static_cast<std::uint8_t>(((sprite_eval_temp_ & 0xCC) >> 2) | ((sprite_eval_temp_ & 0x33) << 2));
                    sprite_eval_temp_ = static_cast<std::uint8_t>(((sprite_eval_temp_ & 0xAA) >> 1) | ((sprite_eval_temp_ & 0x55) << 1));
                }
                sprite_pattern_low_shift_[((ppuDot - 257) >> 3)] = sprite_eval_temp_;
                break;
            case 6:
                ppu_address_bus_ += 8;
                break;
            case 7:
                sprite_eval_temp_ = ppu_directRead(ppu_address_bus_);
                if (ppuScanline == 261) {
                    sprite_eval_temp_ = 0;
                }

                if (((sprite_attr_[((ppuDot - 257) >> 3)] >> 6) & 1) == 1) {
                    sprite_eval_temp_ = static_cast<std::uint8_t>(((sprite_eval_temp_ & 0xF0) >> 4) | ((sprite_eval_temp_ & 0xF) << 4));
                    sprite_eval_temp_ = static_cast<std::uint8_t>(((sprite_eval_temp_ & 0xCC) >> 2) | ((sprite_eval_temp_ & 0x33) << 2));
                    sprite_eval_temp_ = static_cast<std::uint8_t>(((sprite_eval_temp_ & 0xAA) >> 1) | ((sprite_eval_temp_ & 0x55) << 1));
                }
                sprite_pattern_high_shift_[((ppuDot - 257) >> 3)] = sprite_eval_temp_;
                break;
        }
    }
}