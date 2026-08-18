#include "cpu.hpp"
#include <array>
#include <algorithm>
#include <iostream>

int main() {
    CPU cpu;
    cpu.reset();
    constexpr std::array<std::uint8_t, 9> program{
        0xA9, 0x41, 0xAA, 0xE8, 0x8A, 0x85, 0x10, 0xEA, 0xEA
    };
    std::copy(program.begin(), program.end(), cpu.memory_.begin() + 0x8000);
    unsigned cycles = 0;
    for (int i = 0; i < 7; ++i) {
        cycles += cpu.step();
    }
    if (cpu.memory_[0x10] != 0x42 || cycles != 15) {
        std::cerr << "CPU smoke check failed\n";
        return 1;
    }
    std::cout << "CPU smoke check passed\n";
}
