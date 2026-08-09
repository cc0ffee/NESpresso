#pragma once
#include <array>
#include <cstdint>

class CPU {
public:
    void reset();
    std::array<std::uint8_t, 65536> memory_{};
private:
    std::uint8_t reg_a_{0};
    std::uint8_t reg_x_{0};
    std::uint8_t reg_y_{0};
    std::uint16_t pc_{0x8000};
    std::uint8_t status_{0x24};
};
