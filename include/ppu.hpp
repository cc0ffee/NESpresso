#pragma once
#include "cartridge.hpp"
#include <array>

class PPU {
public:
    PPU(Cartridge& cartridge);
    void ppu_writeReg(uint16_t addr, std::uint8_t val);
    std::uint8_t ppu_read(uint16_t addr);
    std::uint8_t ctrl_{};
    std::array<std::uint8_t, 2048> nametable_ram_{};
    std::uint8_t ppu_read_buffer_{};
    std::uint8_t status_{};
    std::array<std::uint8_t, 32> palette_ram_{};

    std::array<std::uint8_t, 32> secondary_oam_{};

    std::uint16_t sprite_eval_oam_addr_{0};
    std::uint8_t secondary_oam_addr_{0};
    std::uint8_t sprite_eval_temp_{0xFF};
    std::uint8_t sprite_eval_tick_{0};

    std::array<std::uint8_t, 8> sprite_pattern_low_shift_{};
    std::array<std::uint8_t, 8> sprite_pattern_high_shift_{};

    std::array<std::uint8_t, 8> sprite_x_pos_{};
    std::array<std::uint8_t, 8> sprite_y_pos_{};
    std::array<std::uint8_t, 8> sprite_tile_{};
    std::array<std::uint8_t, 8> sprite_attr_{};

    std::uint8_t next_sprite_count_{0};
    bool next_scanline_contains_sprite_zero_{false};

    void spriteEvaluation();

    
    std::array<std::uint8_t, 256> oam_data_{};

    void step();

    int ppuDot{0};
    int ppuScanline{0};

    bool enableNMI{false};

    bool frame_ready_{false};

    static constexpr int screen_width = 256;
    static constexpr int screen_height = 240;
    std::array<std::uint8_t, 256 * 240> framebuffer_{};

    std::uint8_t sprite_count_{0};
    bool scanline_contains_sprite_zero_{false};

private:
    Cartridge& cartridge_;

    std::uint8_t ppu_directRead(uint16_t addr);

    std::uint8_t mask_{};
    std::uint8_t oam_addr_{};

    std::uint16_t bg_pattern_low_shift_{};
    std::uint16_t bg_pattern_high_shift_{};
    std::uint16_t bg_attribute_low_shift_{};
    std::uint16_t bg_attribute_high_shift_{};

    std::uint16_t bg_pattern_low_bitplane_{};
    std::uint16_t bg_pattern_high_bitplane_{};

    std::uint16_t tile_attr_{};
    std::uint16_t ppu_address_bus_{};
    std::uint16_t ppu_temp_{};

    std::uint16_t ppu_nextChar{};

    std::uint8_t ppu_scroll_fine_x_{};

    std::uint8_t open_bus_{0};

    std::uint16_t reg_v_{};
    std::uint16_t reg_t_{};
    bool reg_w_{false};

    std::uint32_t open_bus_decay_{0};

    void incScrollY();
    void resetXScroll();
    void resetYScroll();
};